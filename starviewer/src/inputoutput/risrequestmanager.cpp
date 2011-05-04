#include "risrequestmanager.h"

#include <QString>
#include <QMessageBox>
#include <QHash>
#include <QThread>

#include "inputoutputsettings.h"
#include "starviewerapplication.h"
#include "pacsmanager.h"
#include "pacsdevicemanager.h"
#include "patient.h"
#include "study.h"
#include "logging.h"
#include "qpopuprisrequestsscreen.h"
#include "querypacsjob.h"
#include "retrievedicomfilesfrompacsjob.h"

namespace udg {

RISRequestManager::RISRequestManager(PacsManager *pacsManager)
{
    m_pacsManager = pacsManager;
}

RISRequestManager::~RISRequestManager()
{
    //Com que la classe que escolta les peticions del RIS s'executa en un thread li emetem un signal indicant-li que s'ha de parar.
    emit stopListenRISRequests();

    m_listenRISRequestsQThread->exit();
    m_listenRISRequestsQThread->wait();

    m_listenRISRequests->deleteLater();

    delete m_listenRISRequestsQThread;
    delete m_qpopUpRISRequestsScreen;
}

void RISRequestManager::initialize()
{
    m_listenRISRequestsQThread = new QThread();
    m_listenRISRequests = new ListenRISRequests();

    //La classe ListenRISRequests necessita el seu propi thread perquè sempre està executant-se esperant noves peticions, si l'executes el thread principal
    //Starviewer quedaria congelada només escoltant peticions del RIS
    m_listenRISRequests->moveToThread(m_listenRISRequestsQThread);
    m_listenRISRequestsQThread->start();

    m_qpopUpRISRequestsScreen = new QPopUpRISRequestsScreen();

    createConnections();
}

void RISRequestManager::createConnections()
{
    connect(m_listenRISRequests, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(processRISRequest(DicomMask)));
    connect(m_listenRISRequests, SIGNAL(errorListening(ListenRISRequests::ListenRISRequestsError)), SLOT(showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError)));
    /**Hem d'indica a la classe ListenRISRequests que pot començar a escoltar/parar peticions a través d'un signal, perquè si ho fèssim invocant el mètode listen()
       o stopListen directament aquest seria executat pel thread que l'invoca i podria fer petar l'aplicaciño, en canvi amb un signal aquest és atés pel thread
       al que pertany ListenRISRequests*/
    connect(this, SIGNAL(listenRISRequests()), m_listenRISRequests, SLOT(listen()));
    connect(this, SIGNAL(stopListenRISRequests()), m_listenRISRequests, SLOT(stopListen()));
}

void RISRequestManager::listen()
{
    initialize();

    emit listenRISRequests();
}

void RISRequestManager::processRISRequest(DicomMask dicomMaskRISRequest)
{
    INFO_LOG("Encuem sol·licitud de descàrrega d'un estudi del RIS amb accession number " + dicomMaskRISRequest.getAccessionNumber());
    //Per anar atenent les descàrregues a mesura que ens arriben encuem les peticions, només fem una cerca al PACS a la vegada, una
    //vegada hem trobat l'estudi en algun PACS, es posa a descarregar i s'aten una altra petició
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
    //Al iniciar una nova consulta netegem la llista UID d'estudis demanats per descarregar i pendents de descarregar
    //TODO:Si ens arriba una altre petició del RIS mentre encara descarreguem l'anterior petició no es farà seguiment de la descarrega actual, sinó de la
    //última que ha arribat
    m_studiesInstancesUIDRequestedToRetrieve.clear();

    // TODO Ara mateix cal que nosaltres mateixos fem aquesta comprovació però potser seria interessant que el mètode PACSDevicemanager::queryStudy()
    // fes aquesta comprovació i ens retornes algun codi que pugui descriure com ha anat la consulta i així poder actuar en conseqüència mostrant
    // un message box, fent un log o el que calgui segons la ocasió.
    QList<PacsDevice> queryablePACS = PacsDeviceManager().getPACSList(PacsDeviceManager::PacsWithQueryRetrieveServiceEnabled, true);
    if (queryablePACS.isEmpty())
    {
        QMessageBox::information(0, ApplicationNameString, tr("A RIS request has been received, but It could not be performed because there are not configured default PACS to query.") + "\n\n" + tr("Please, check your PACS settings."));
        INFO_LOG("No s'ha pogut processar la peticio del RIS perque no hi ha PACS configurats per cercar per defecte");
        m_queueRISRequests.dequeue();
        return;
    }

    // Mostrem el popUP amb l'accession number
    m_qpopUpRISRequestsScreen->queryStudiesByAccessionNumberStarted();
    m_qpopUpRISRequestsScreen->activateWindow();
    m_qpopUpRISRequestsScreen->show();

    foreach (const PacsDevice &pacsDevice, queryablePACS)
    {
        enqueueQueryPACSJobToPACSManagerAndConnectSignals(new QueryPacsJob(pacsDevice, maskRISRequest, QueryPacsJob::study));
    }
}

void RISRequestManager::enqueueQueryPACSJobToPACSManagerAndConnectSignals(QueryPacsJob *queryPACSJob)
{
    connect(queryPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(queryPACSJobFinished(PACSJob*)));
    connect(queryPACSJob, SIGNAL(PACSJobCancelled(PACSJob*)), SLOT(queryPACSJobCancelled(PACSJob*)));
    m_queryPACSJobPendingExecuteOrExecuting.insert(queryPACSJob->getPACSJobID(), queryPACSJob);

    m_pacsManager->enqueuePACSJob(queryPACSJob);
}

void RISRequestManager::queryPACSJobFinished(PACSJob *pacsJob)
{
    QueryPacsJob *queryPACSJob = qobject_cast<QueryPacsJob*>(pacsJob);

    if (queryPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un QueryPACSJob");
    }
    else
    {
        if (queryPACSJob->getStatus() == PACSRequestStatus::QueryOk)
        {
            retrieveFoundStudiesFromPACS(queryPACSJob);
        }
        else if (queryPACSJob->getStatus() != PACSRequestStatus::QueryCancelled)
        {
            ERROR_LOG(QString("S'ha produit un error al cercar estudis al PACS %1 per la sol.licitud del RIS").arg(queryPACSJob->getPacsDevice().getAETitle()));
            errorQueryingStudy(queryPACSJob);
        }

        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

        //Fem un deleteLater per si algú més ha capturat el signal de PACSJobFinished per aquest aquest job no es trobi l'objecte destruït
        queryPACSJob->deleteLater();

        if (m_queryPACSJobPendingExecuteOrExecuting.isEmpty())
        {
            queryRequestRISFinished();
        }
    }
}

void RISRequestManager::queryPACSJobCancelled(PACSJob *pacsJob)
{
    //Aquest slot també serveix per si alguna altre classe ens cancel·la un PACSJob nostre per a que ens n'assabentem

    QueryPacsJob *queryPACSJob = qobject_cast<QueryPacsJob*>(pacsJob);

    if (queryPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que s'ha cancel·lat no és un QueryPACSJob");
    }
    else
    {
        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

        //Fem un deleteLater per si algú més ha capturat el signal de PACSJobFinished per aquest aquest job no es trobi l'objecte destruït
        queryPACSJob->deleteLater();

        if (m_queryPACSJobPendingExecuteOrExecuting.isEmpty())
        {
            queryRequestRISFinished();
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

        m_qpopUpRISRequestsScreen->showNotStudiesFoundMessage();
        QMessageBox::information(NULL, ApplicationNameString, message);
    }

    if (m_queueRISRequests.count() > 0)
    {
        INFO_LOG("Hi ha més sol·licituts de RIS pendent d'executar");
        //Tenim altres sol·licituds del RIS per descarregar, les processem
        queryPACSRISStudyRequest(m_queueRISRequests.head());
    }
}

void RISRequestManager::errorQueryingStudy(QueryPacsJob *queryPACSJob)
{
    QString errorMessage = tr("Processing the RIS request, can't query PACS %1 from %2.\nBe sure that the IP and AETitle of It are correct.")
        .arg(queryPACSJob->getPacsDevice().getAETitle())
        .arg(queryPACSJob->getPacsDevice().getInstitution());

    QMessageBox::critical(NULL, ApplicationNameString, errorMessage);
}

void RISRequestManager::retrieveFoundStudiesFromPACS(QueryPacsJob *queryPACSJob)
{
    foreach (Patient *patient, queryPACSJob->getPatientStudyList())
    {
        foreach (Study *study, patient->getStudies())
        {
            if (!m_studiesInstancesUIDRequestedToRetrieve.contains(study->getInstanceUID()))
            {
                INFO_LOG(QString("S'ha trobat estudi que compleix criteri de cerca del RIS. Estudi UID %1, PacsId %2").arg(study->getInstanceUID(), queryPACSJob->getHashTablePacsIDOfStudyInstanceUID()[study->getInstanceUID()]));

                //Descarreguem l'estudi trobat
                RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = retrieveStudy(queryPACSJob->getHashTablePacsIDOfStudyInstanceUID()[study->getInstanceUID()], study);

                if (Settings().getValue(InputOutputSettings::RISRequestViewOnceRetrieved).toBool())
                {
                    //TODO: Això és una mica lleig haver de controlar des d'aquí que fe amb l'estudi una vegada descarregat, no es podria posar com a
                    //propietat al Job, i centralitzar-ho a un responsable que fos l'encarregat de fer l'acció pertinent
                    if (m_studiesInstancesUIDRequestedToRetrieve.count() == 0)
                    {
                        //El primer estudi que descarreguem trobat d'una petició del RIS fem un retrieve&view, pels altres serà un retrieve&Load
                        m_pacsJobIDToViewWhenFinished.append(retrieveDICOMFilesFromPACSJob->getPACSJobID());
                    }
                    else
                    {
                        m_pacsJobIDToLoadWhenFinished.append(retrieveDICOMFilesFromPACSJob->getPACSJobID());
                    }
                }

                m_studiesInstancesUIDRequestedToRetrieve.append(study->getInstanceUID());
            }
            else
            {
                WARN_LOG(QString("S'ha trobat l'estudi UID %1 del PACS Id %2 que coincidieix amb els parametres del cerca del RIS, pero ja s'ha demanat descarregar-lo d'un altre PACS.").arg(study->getInstanceUID(), queryPACSJob->getHashTablePacsIDOfStudyInstanceUID()[study->getInstanceUID()]));
            }
        }
    }
}

RetrieveDICOMFilesFromPACSJob* RISRequestManager::retrieveStudy(QString pacsIDToRetrieve, Study *study)
{
    PacsDevice pacsDevice = PacsDeviceManager().getPACSDeviceByID(pacsIDToRetrieve);
    DicomMask studyToRetrieveDICOMMask;
    studyToRetrieveDICOMMask.setStudyInstanceUID(study->getInstanceUID());

    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = new RetrieveDICOMFilesFromPACSJob(pacsDevice, study, studyToRetrieveDICOMMask, RetrieveDICOMFilesFromPACSJob::Medium);
    m_qpopUpRISRequestsScreen->addStudyToRetrieveByAccessionNumber(retrieveDICOMFilesFromPACSJob);
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobFinished(PACSJob *)));
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobCancelled(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobCancelled(PACSJob *)));

    m_pacsManager->enqueuePACSJob(retrieveDICOMFilesFromPACSJob);

    return retrieveDICOMFilesFromPACSJob;
}

void RISRequestManager::retrieveDICOMFilesFromPACSJobCancelled(PACSJob *pacsJob)
{
    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = qobject_cast<RetrieveDICOMFilesFromPACSJob*>(pacsJob);

    if (retrieveDICOMFilesFromPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un RetrieveDICOMFilesFromPACSJob");
    }
    else
    {
        INFO_LOG(QString("La descarrega de l'estudi %1 del PACS %2 sol.licitada pel RIS ha estat cancel.lada")
            .arg(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID())
            .arg(retrieveDICOMFilesFromPACSJob->getPacsDevice().getAETitle()));
    }
}

///Slot que s'activa quan un job de descarrega d'una petició del RIS ha finalitzat
void RISRequestManager::retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob)
{
    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = qobject_cast<RetrieveDICOMFilesFromPACSJob*>(pacsJob);

    if (retrieveDICOMFilesFromPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un RetrieveDICOMFilesFromPACSJob");
    }
    else
    {
        if (retrieveDICOMFilesFromPACSJob->getStatus() != PACSRequestStatus::RetrieveOk)
        {
            if (retrieveDICOMFilesFromPACSJob->getStatus() == PACSRequestStatus::RetrieveSomeDICOMFilesFailed)
            {
                QMessageBox::warning(NULL, ApplicationNameString, retrieveDICOMFilesFromPACSJob->getStatusDescription());
            }
            else
            {
                QMessageBox::critical(NULL, ApplicationNameString, retrieveDICOMFilesFromPACSJob->getStatusDescription());
                return;
            }
        }

        if (m_pacsJobIDToViewWhenFinished.removeOne(retrieveDICOMFilesFromPACSJob->getPACSJobID()))
        {
            emit viewStudyRetrievedFromRISRequest(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());
        }
        else if (m_pacsJobIDToLoadWhenFinished.removeOne(retrieveDICOMFilesFromPACSJob->getPACSJobID()))
        {
            emit loadStudyRetrievedFromRISRequest(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());
        }

        /*Com que l'objecte és un punter altres classes poden haver capturat el Signal per això li fem un deleteLater() en comptes d'un delete, per evitar
          que quan responguin al signal es trobin que l'objecte ja no existeix. L'objecte serà destruït per Qt quan es retorni el eventLoop*/
        retrieveDICOMFilesFromPACSJob->deleteLater();
    }
}

void RISRequestManager::showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error)
{
    QString message;
    int risPort = Settings().getValue(InputOutputSettings::RISRequestsPort).toInt();

    switch (error)
    {
        case ListenRISRequests::RisPortInUse:
            message = tr("Can't listen RIS requests on port %1, the port is in use by another application.").arg(risPort);
            break;
        case ListenRISRequests::UnknownNetworkError:
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(risPort);
            message += tr("\nIf the problem persists contact with an administrator.");
            break;
    }

    QMessageBox::critical(NULL, ApplicationNameString, message);
}

};
