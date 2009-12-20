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

    //Inicialitzem a fals indicant que pel moment no s'ha trobat cap estudi que compleixi amb la màscara de cerca enviada pel RIS
    m_foundRISRequestStudy = false;
    m_qpopUpRisRequestsScreen->setAccessionNumber(maskRISRequest.getAccessionNumber()); //Mostrem el popUP amb l'accession number

    m_qpopUpRisRequestsScreen->show();

    m_pacsManager->queryStudy(maskRISRequest, PacsDeviceManager().getPACSList(PacsDeviceManager::PacsWithQueryRetrieveServiceEnabled, true));
}

void RISRequestManager::queryStudyResultsReceived(QList<Patient*> patientsList, QHash<QString, QString> hashTablePacsIDOfStudyInstanceUID)
{
    foreach(Patient *patient, patientsList)
    {
        foreach(Study *study, patient->getStudies())
        {
            INFO_LOG(QString("S'ha trobat estudi que compleix criteri de cerca del RIS. Estudi UID %1 , PacsId %2").arg( study->getInstanceUID(), hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()]));

            //TODO Aquesta classe és la que hauria de tenir la responsabilitat de descarregar l'estudi
            emit retrieveStudyFromRISRequest(hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()] , study);
            m_foundRISRequestStudy = true;
        }
    }
}

void RISRequestManager::queryRequestRISFinished()
{
    DicomMask dicomMaskRISRequest = m_queueRISRequests.dequeue();

    INFO_LOG("Ha acabat la cerca dels estudis sol·licitats pel RIS amb l'Accession number " + dicomMaskRISRequest.getAccessionNumber());

    if (!m_foundRISRequestStudy)
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

    errorMessage = tr("Processing the RIS request, can't query PACS %1 from %2.\nBe sure that the IP and AETitle of this PACS are correct")
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
            message += tr("\n\nIf the error has ocurred when openned new %1's windows, close this window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);
            break;
        case ListenRISRequestThread::UnknownNetworkError :
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(risPort);
            message += tr("\nIf the problem persist contact with an administrator.");
            break;
    }

    QMessageBox::critical(NULL, ApplicationNameString, message);
}

};

