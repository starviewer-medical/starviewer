#include "risrequestmanager.h"

#include <QString>
#include <QMessageBox>
#include <QHash>

#include "inputoutputsettings.h"
#include "starviewerapplication.h"
#include "pacsmanager.h"
#include "pacsdevicemanager.h"
#include "pacsdevice.h"
#include "patient.h"
#include "study.h"
#include "logging.h"

namespace udg{

RISRequestManager::~RISRequestManager()
{
    delete m_listenRISRequestThread;
}

void RISRequestManager::initialize()
{
    Settings settings;

    m_listenRISRequestThread = new ListenRISRequestThread(this);

    if (settings.getValue(InputOutputSettings::ListenToRISRequests).toBool()) 
        m_qpopUpRisRequestsScreen = new QPopUpRisRequestsScreen();

    m_pacsManager = new PacsManager();

    createConnections();
}

void RISRequestManager::createConnections()
{
    connect(m_listenRISRequestThread, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(processRISRequest(DicomMask)));
    connect(m_listenRISRequestThread, SIGNAL(errorListening(ListenRISRequestThread::ListenRISRequestThreadError)), SLOT(showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError)));

    connect(m_pacsManager, SIGNAL(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)), SLOT(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)));
    connect(m_pacsManager, SIGNAL(errorQueryingStudy(PacsDevice)), SLOT(errorQueryingStudy(PacsDevice)));
    connect(m_pacsManager, SIGNAL(queryFinished()), SLOT(queryRequestRISFinished()));
}

void RISRequestManager::listen()
{
    initialize();

    m_listenRISRequestThread->listen();
}

void RISRequestManager::processRISRequest(DicomMask dicomMaskRISRequest)
{
    INFO_LOG("Encuem sol·licitud de descàrrega d'un estudi del RIS amb accession number " + dicomMaskRISRequest.getAccessionNumber());
    m_queueRISRequests.enqueue(dicomMaskRISRequest);

    //Si tenim més d'un element ja hi ha un altre consulta d'un RIS Executant-se per tant no fem res
    if (m_queueRISRequests.count() == 1)
    {
        queryPACSRISStudyRequest(m_queueRISRequests.head());
    }
}

void RISRequestManager::queryPACSRISStudyRequest(DicomMask maskRISRequest)
{
    INFO_LOG("Comencem a cercar l'estudi sol·licitat pel RIS amb accession number " + maskRISRequest.getAccessionNumber());
    //Al iniciar una nova consulta netegem la llista UID d'estudis demanats per descarregar
    m_studiesInstancesUIDRequestedToRetrieve.clear();

    // Mostrem el popUP amb l'accession number
    m_qpopUpRisRequestsScreen->setAccessionNumber(maskRISRequest.getAccessionNumber());
    m_qpopUpRisRequestsScreen->show();

    // TODO Ara mateix cal que nosaltres mateixos fem aquesta comprovació però potser seria interessant que el mètode PACSDevicemanager::queryStudy()
    // fes aquesta comprovació i ens retornes algun codi que pugui descriure com ha anat la consulta i així poder actuar en conseqüència mostrant 
    // un message box, fent un log o el que calgui segons la ocasió.
    QList<PacsDevice> queryablePACS = PacsDeviceManager().getPACSList(PacsDeviceManager::PacsWithQueryRetrieveServiceEnabled, true);
    if ( queryablePACS.isEmpty() )
    {
        QMessageBox::information(0, ApplicationNameString, tr("The RIS request could not be performed.") + "\n\n" + tr("There are no configured PACS to query.") + "\n" + tr("Please, check your PACS settings.") );
    }
    else
    {
        m_pacsManager->queryStudy(maskRISRequest, queryablePACS);
    }
}

void RISRequestManager::queryStudyResultsReceived(QList<Patient*> patientsList, QHash<QString, QString> hashTablePacsIDOfStudyInstanceUID)
{
    foreach(Patient *patient, patientsList)
    {
        foreach(Study *study, patient->getStudies())
        {
            if (!m_studiesInstancesUIDRequestedToRetrieve.contains(study->getInstanceUID()))
            {
                INFO_LOG(QString("S'ha trobat estudi que compleix criteri de cerca del RIS. Estudi UID %1 , PacsId %2").arg( study->getInstanceUID(), hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()]));

                //TODO Aquesta classe és la que hauria de tenir la responsabilitat de descarregar l'estudi
                emit retrieveStudyFromRISRequest(hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()] , study);
                m_studiesInstancesUIDRequestedToRetrieve.append(study->getInstanceUID());
            }
            else
            {
                WARN_LOG(QString("S'ha trobat l'estudi UID %1 del PACS Id %2 que coincidieix amb els parametres del cerca del RIS, pero ja s'ha demanat descarregar-lo d'un altre PACS.").arg(study->getInstanceUID(), hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()]));
            }
        }
    }
}

void RISRequestManager::queryRequestRISFinished()
{
    DicomMask dicomMaskRISRequest = m_queueRISRequests.dequeue();

    INFO_LOG("Ha acabat la cerca dels estudis sol·licitats pel RIS amb l'Accession number " + dicomMaskRISRequest.getAccessionNumber());

    if (m_studiesInstancesUIDRequestedToRetrieve.count() == 0)
    {
        INFO_LOG("No s'ha trobat cap estudi sol·licitat pel RIS amb l'accession number " + dicomMaskRISRequest.getAccessionNumber());
        //Si no hem trobat cap estudi que coincideix llancem MessageBox
        QString message = tr("%2 can't execute the RIS request, because hasn't found the Study with accession number %1 in the default PACS.").arg(dicomMaskRISRequest.getAccessionNumber(), ApplicationNameString);

        QMessageBox::information(NULL , ApplicationNameString , message);
    }

    if (m_queueRISRequests.count() > 0)
    {
        INFO_LOG("Hi ha més sol·licituts de RIS pendent d'executar");
        //Tenim altres sol·licituds del RIS per descarregar, les processem
        queryPACSRISStudyRequest(m_queueRISRequests.head());
    }
}

void RISRequestManager::errorQueryingStudy(PacsDevice pacsDeviceError)
{
    QString errorMessage;

    errorMessage = tr("Processing the RIS request, can't query PACS %1 from %2.\nBe sure that the IP and AETitle of It are correct.")
        .arg(pacsDeviceError.getAETitle())
        .arg(pacsDeviceError.getInstitution());

    QMessageBox::critical(NULL, ApplicationNameString, errorMessage);
}

void RISRequestManager::showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError error)
{
    QString message;
    Settings settings;
    int risPort = settings.getValue( InputOutputSettings::RisRequestsPort ).toInt();
    switch(error)
    {
        case ListenRISRequestThread::RisPortInUse :
            message = tr("Can't listen RIS requests on port %1, the port is in use by another application.").arg(risPort);
            break;
        case ListenRISRequestThread::UnknownNetworkError :
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(risPort);
            message += tr("\nIf the problem persists contact with an administrator.");
            break;
    }

    QMessageBox::critical(NULL, ApplicationNameString, message);
}

};

