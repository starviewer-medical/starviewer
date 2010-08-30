#include "risrequestmanager.h"

#include <QString>
#include <QMessageBox>
#include <QHash>
#include <QThread>

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
    m_listenRISRequestsQThread->terminate();
    m_listenRISRequestsQThread->wait();

    delete m_listenRISRequests;
}

void RISRequestManager::initialize()
{
    m_listenRISRequestsQThread = new QThread();
    m_listenRISRequests = new ListenRISRequests(this);

    //La classe ListenRISRequests necessita el seu propi thread perquè sempre està executant-se esperant noves peticions, si l'executes el thread principal
    //Starviewer quedaria congelada només escoltant peticions del RIS
    m_listenRISRequests->moveToThread(m_listenRISRequestsQThread);
    m_listenRISRequestsQThread->start();

    m_qpopUpRisRequestsScreen = new QPopUpRisRequestsScreen();

    m_pacsManager = new PacsManager();

    createConnections();
}

void RISRequestManager::createConnections()
{
    connect(m_listenRISRequests, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(processRISRequest(DicomMask)));
    connect(m_listenRISRequests, SIGNAL(errorListening(ListenRISRequests::ListenRISRequestsError)), SLOT(showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError)));
    /**Hem d'indica a la classe ListenRISRequests que pot començar a escoltar peticions a través d'un signal, perquè si ho fèssim invocant el mètode listen() directament
       aquest seria executat pel thread que l'invoca, en canvi amb un signal aquest és atés pel thread al que pertany ListenRISRequests*/
    connect(this, SIGNAL(listenRISRequests()), m_listenRISRequests, SLOT(listen()));

    connect(m_pacsManager, SIGNAL(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)), SLOT(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)));
    connect(m_pacsManager, SIGNAL(errorQueryingStudy(PacsDevice)), SLOT(errorQueryingStudy(PacsDevice)));
    connect(m_pacsManager, SIGNAL(queryFinished()), SLOT(queryRequestRISFinished()));
}

void RISRequestManager::listen()
{
    initialize();

    emit listenRISRequests();
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
    // Inicialitzem a fals indicant que pel moment no s'ha trobat cap estudi que compleixi amb la màscara de cerca enviada pel RIS
    m_foundRISRequestStudy = false;

    // Mostrem el popUP amb l'accession number
    m_qpopUpRisRequestsScreen->setAccessionNumber(maskRISRequest.getAccessionNumber());
    m_qpopUpRisRequestsScreen->activateWindow();
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
            /*Degut al bug que es descriu al ticket #1042, es fa que només es descarregui el primer estudi trobat a la cerca de PACS
              Si trobem més d'un estudi que compleixi la cerca, es descarrega el primer i executem la pipeline per carregar l'estudi i visualitzar-lo, de mentres
    	      s'executa,si el segon és petit i es descarrega ràpidament, executa la pipeline de carregar l'estudi mentre el primer encara l'està executant 
              per carregar i visualitzar l'estudi l'Starviewer peta. Sembla que també hi haurien problemes perquè mentre s'estan passant els fillers del primer 
              estudi descarregat, el segona descàrrega matxaca els fitxers del a primera descàrrega.*/
            if (!m_foundRISRequestStudy)
            {
                INFO_LOG(QString("S'ha trobat estudi que compleix criteri de cerca del RIS. Estudi UID %1 , PacsId %2").arg( study->getInstanceUID(), hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()]));
          
                //TODO Aquesta classe és la que hauria de tenir la responsabilitat de descarregar l'estudi
                emit retrieveStudyFromRISRequest(hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()] , study);
                m_foundRISRequestStudy = true;
            }
            else
            {
                WARN_LOG(QString("S'ha trobat l'estudi UID %1 del PACS Id %2 que coincidieix amb els parametres del cerca del RIS, pero nomes es baixara el primer estudi trobat").arg(study->getInstanceUID(), hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()]));
            }
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

    errorMessage = tr("Processing the RIS request, can't query PACS %1 from %2.\nBe sure that the IP and AETitle of It are correct.")
        .arg(pacsDeviceError.getAETitle())
        .arg(pacsDeviceError.getInstitution());

    QMessageBox::critical(NULL, ApplicationNameString, errorMessage);
}

void RISRequestManager::showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error)
{
    QString message;
    Settings settings;
    int risPort = settings.getValue( InputOutputSettings::RISRequestsPort ).toInt();
    switch(error)
    {
        case ListenRISRequests::RisPortInUse :
            message = tr("Can't listen RIS requests on port %1, the port is in use by another application.").arg(risPort);
            break;
        case ListenRISRequests::UnknownNetworkError :
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(risPort);
            message += tr("\nIf the problem persists contact with an administrator.");
            break;
    }

    QMessageBox::critical(NULL, ApplicationNameString, message);
}

};

