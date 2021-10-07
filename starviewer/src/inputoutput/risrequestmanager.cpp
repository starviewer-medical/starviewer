/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
#include "localdatabasemanager.h"
#include "qmessageboxautoclose.h"
#include "usermessage.h"

namespace udg {

const int RISRequestManager::secondsTimeOutToHidePopUpAndAutoCloseQMessageBox = 5;

RISRequestManager::RISRequestManager()
{
}

RISRequestManager::~RISRequestManager()
{
    // Com que la classe que escolta les peticions del RIS s'executa en un thread li emetem un signal indicant-li que s'ha de parar.
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

    // La classe ListenRISRequests necessita el seu propi thread perquè sempre està executant-se esperant noves peticions, si l'executes el thread principal
    // Starviewer quedaria congelada només escoltant peticions del RIS
    m_listenRISRequests->moveToThread(m_listenRISRequestsQThread);
    m_listenRISRequestsQThread->start();

    m_qpopUpRISRequestsScreen = new QPopUpRISRequestsScreen();
    m_qpopUpRISRequestsScreen->setTimeOutToHidePopUpAfterStudiesHaveBeenRetrieved(secondsTimeOutToHidePopUpAndAutoCloseQMessageBox * 1000);

    createConnections();
}

void RISRequestManager::createConnections()
{
    connect(m_listenRISRequests, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(processRISRequest(DicomMask)));
    connect(m_listenRISRequests, SIGNAL(errorListening(ListenRISRequests::ListenRISRequestsError)),
            SLOT(showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError)));
    // Hem d'indica a la classe ListenRISRequests que pot començar a escoltar/parar peticions a través d'un signal, perquè si ho fèssim invocant el mètode
    // listen() o stopListen directament aquest seria executat pel thread que l'invoca i podria fer petar l'aplicaciño, en canvi amb un signal aquest és
    // atés pel thread al que pertany ListenRISRequests
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
    // Per anar atenent les descàrregues a mesura que ens arriben encuem les peticions, només fem una cerca al PACS a la vegada, una
    // vegada hem trobat l'estudi en algun PACS, es posa a descarregar i s'aten una altra petició
    m_queueRISRequests.enqueue(dicomMaskRISRequest);
    m_accessionNumberOfLastRequest = dicomMaskRISRequest.getAccessionNumber();
    m_signalViewStudyEmittedForLastRISRequest = false;

    // Si tenim més d'un element ja hi ha un altre consulta d'un RIS Executant-se per tant no fem res
    if (m_queueRISRequests.count() == 1)
    {
        queryPACSRISStudyRequest(m_queueRISRequests.head());
    }
}

void RISRequestManager::queryPACSRISStudyRequest(DicomMask maskRISRequest)
{
    INFO_LOG("Comencem a cercar l'estudi sol·licitat pel RIS amb accession number " + maskRISRequest.getAccessionNumber());
    m_numberOfStudiesAddedToRetrieveForCurrentRisRequest = 0;
    // Al iniciar una nova consulta netegem la llista UID d'estudis demanats per descarregar i pendents de descarregar
    // TODO:Si ens arriba una altre petició del RIS mentre encara descarreguem l'anterior petició no es farà seguiment de la descarrega actual, sinó de la
    // última que ha arribat
    m_studiesInstancesUIDRequestedToRetrieve.clear();

    m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeenRetrievedAgain = false;
    m_studiesInDatabaseHaveToBeenRetrievedAgain = false;

    // TODO Ara mateix cal que nosaltres mateixos fem aquesta comprovació però potser seria interessant que el mètode PACSDevicemanager::queryStudy()
    // fes aquesta comprovació i ens retornes algun codi que pugui descriure com ha anat la consulta i així poder actuar en conseqüència mostrant
    // un message box, fent un log o el que calgui segons la ocasió.
    QList<PacsDevice> queryablePACS =
            PacsDeviceManager::getPacsList(PacsDeviceManager::DimseWithQueryRetrieveService | PacsDeviceManager::Wado | PacsDeviceManager::OnlyDefault);
    if (queryablePACS.isEmpty())
    {
        QMessageBox::information(0, ApplicationNameString, tr("Cannot retrieve the studies requested from RIS because there is no configured default "
                                                              "PACS to query.") + "\n\n" + tr("Please, check your PACS settings."));
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
        enqueueQueryPACSJobToPACSManagerAndConnectSignals(PACSJobPointer(new QueryPacsJob(pacsDevice, maskRISRequest, QueryPacsJob::study)));
    }
}

void RISRequestManager::enqueueQueryPACSJobToPACSManagerAndConnectSignals(PACSJobPointer queryPACSJob)
{
    connect(queryPACSJob.data(), SIGNAL(PACSJobFinished(PACSJobPointer)), SLOT(queryPACSJobFinished(PACSJobPointer)));
    connect(queryPACSJob.data(), SIGNAL(PACSJobCancelled(PACSJobPointer)), SLOT(queryPACSJobCancelled(PACSJobPointer)));
    m_queryPACSJobPendingExecuteOrExecuting.insert(queryPACSJob->getPACSJobID(), queryPACSJob);

    PacsManagerSingleton::instance()->enqueuePACSJob(queryPACSJob);
}

void RISRequestManager::queryPACSJobFinished(PACSJobPointer pacsJob)
{
    QSharedPointer<QueryPacsJob> queryPACSJob = pacsJob.objectCast<QueryPacsJob>();

    if (queryPACSJob.isNull())
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un QueryPACSJob");
    }
    else
    {
        if (queryPACSJob->getStatus() == PACSRequestStatus::QueryOk)
        {
            addFoundStudiesToRetrieveQueue(pacsJob);
        }
        else if (queryPACSJob->getStatus() != PACSRequestStatus::QueryCancelled)
        {
            ERROR_LOG(QString("S'ha produit un error al cercar estudis al PACS %1 per la sol.licitud del RIS")
                         .arg(queryPACSJob->getPacsDevice().getAETitle()));
            errorQueryingStudy(pacsJob);
        }

        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

        if (m_queryPACSJobPendingExecuteOrExecuting.isEmpty())
        {
            queryRequestRISFinished();
        }
    }
}

void RISRequestManager::queryPACSJobCancelled(PACSJobPointer pacsJob)
{
    // Aquest slot també serveix per si alguna altre classe ens cancel·la un PACSJob nostre per a que ens n'assabentem

    QSharedPointer<QueryPacsJob> queryPACSJob = pacsJob.objectCast<QueryPacsJob>();

    if (queryPACSJob.isNull())
    {
        ERROR_LOG("El PACSJob que s'ha cancel·lat no és un QueryPACSJob");
    }
    else
    {
        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

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

    if (m_numberOfStudiesAddedToRetrieveForCurrentRisRequest == 0)
    {
        INFO_LOG("No s'ha trobat cap estudi sol·licitat pel RIS amb l'accession number " + dicomMaskRISRequest.getAccessionNumber());
        // Si no hem trobat cap estudi que coincideix llancem MessageBox
        QString message = tr("Unable to execute the RIS request. The study with accession number %1 was not found in the default PACS.")
                        .arg(dicomMaskRISRequest.getAccessionNumber());

        m_qpopUpRISRequestsScreen->showNotStudiesFoundMessage();
        QMessageBox::information(NULL, ApplicationNameString, message);
    }

    if (m_queueRISRequests.count() > 0)
    {
        INFO_LOG("Hi ha més sol·licituts de RIS pendent d'executar");
        // Tenim altres sol·licituds del RIS per descarregar, les processem
        queryPACSRISStudyRequest(m_queueRISRequests.head());
    }
}

void RISRequestManager::errorQueryingStudy(PACSJobPointer queryPACSJob)
{
    QString errorMessage = tr("RIS request error: cannot query PACS %1 from %2.\nMake sure its IP and AE Title are correct.")
        .arg(queryPACSJob->getPacsDevice().getAETitle())
        .arg(queryPACSJob->getPacsDevice().getInstitution());

    QMessageBox::critical(NULL, ApplicationNameString, errorMessage);
}

void RISRequestManager::addFoundStudiesToRetrieveQueue(PACSJobPointer pacsJob)
{
    QSharedPointer<QueryPacsJob> queryPACSJob = pacsJob.objectCast<QueryPacsJob>();

    foreach (Patient *patient, queryPACSJob->getPatientStudyList())
    {
        foreach (Study *study, patient->getStudies())
        {
            if (!m_studiesInstancesUIDRequestedToRetrieve.contains(study->getInstanceUID()))
            {
                INFO_LOG(QString("S'ha trobat estudi que compleix criteri de cerca del RIS. Estudi UID %1, PacsId %2")
                    .arg(study->getInstanceUID(), study->getDICOMSource().getRetrievePACS().at(0).getID()));

                //Quan es posa un estudi a descarregar es comprova si aquest existeix a la BD, si existeix es pregunta al usuari si vol tornar a descarregar els estudis
                //que ja estan en local. Mentre es mostra el QMessageBox preguntant a l'usuari el thread que executa aquesta classe (és el thread de la UI) es continua execuant
                //podent respondre a nous signals, podent passar que arribi un signal d'un altre PACS que ha acabat de fer la cerca, mentre l'usuari no ha respós al QMessageBox no s'ha
                //de fer cap descàrrega a l'espera de que respongui l'usuari. Per això encuem els estudis a descarregar en una cua, simulant un productor consumidor, d'aquesta manera
                //quan l'usuari ha respós es processen els estudis afegits a la cua de descàrrega, sabent que s'ha de fer amb aquests estudis, sinó ens podriem trobar preguntant a l'usuari
                //diverses vegades per una mateixa petició si els estudis que existeixin en local s'han de tornar a descarregar
                m_studiesToRetrieveQueue.enqueue(study);
                m_studiesInstancesUIDRequestedToRetrieve.append(study->getInstanceUID());

                if (m_studiesToRetrieveQueue.count() == 1)
                {
                    //Si no hi havia cap estudi per descarregar engeguem el consumidor
                    retrieveFoundStudiesInQueue();
                }
            }
            else
            {
                //Ja hem trobat l'estudi en un altra PACS pel qual ja s'ha demanat descarregar-lo, no cal tornar-hi.
                WARN_LOG(QString("S'ha trobat l'estudi UID %1 del PACS Id %2 que coincidieix amb els parametres del cerca del RIS, pero ja s'ha demanat "
                             "descarregar-lo d'un altre PACS.")
                        .arg(study->getInstanceUID(), study->getDICOMSource().getRetrievePACS().at(0).getID()));
            }
        }
    }
}

void RISRequestManager::retrieveFoundStudiesInQueue()
{
    while (!m_studiesToRetrieveQueue.isEmpty())
    {
        retrieveStudyFoundInQueryPACS(m_studiesToRetrieveQueue.head());

        //Al esborra els pacients automàticament s'esborren els Study del Patient. Study i Patient hereden de QObject i els objectes Study estan inserits com a fills QObject de Patient.
        //Això fa que quan eliminem Patient s'esborrin els estudis, perquè tot objecte que heredi de QObject quan es destrueix fa un delete dels seus QObject fills, que en aquest cas seria Study
        m_studiesToRetrieveQueue.dequeue()->getParentPatient()->deleteLater();
    }
}

void RISRequestManager::retrieveStudyFoundInQueryPACS(Study *study)
{
    switch (getDICOMSouceFromRetrieveStudy(study))
    {
        case PACS:
            INFO_LOG(QString("L'estudi sol.licitat pel RIS %1 s'obtindra del PACS").arg(study->getInstanceUID())); 
            retrieveStudyFromPACS(study);
            break;
        
        case Database:
            INFO_LOG(QString("L'estudi sol.licitat pel RIS %1 s'obtindra de la base de dades local").arg(study->getInstanceUID())); 
            retrieveStudyFromDatabase(study);
            break;
    }

    m_numberOfStudiesAddedToRetrieveForCurrentRisRequest++;
}

PACSJobPointer RISRequestManager::retrieveStudyFromPACS(Study *study)
{
    PacsDevice pacsDevice = study->getDICOMSource().getRetrievePACS().at(0);

    PACSJobPointer retrieveDICOMFilesFromPACSJob(new RetrieveDICOMFilesFromPACSJob(pacsDevice, RetrieveDICOMFilesFromPACSJob::Medium, study));

    m_qpopUpRISRequestsScreen->addStudyToRetrieveFromPACSByAccessionNumber(retrieveDICOMFilesFromPACSJob);
    connect(retrieveDICOMFilesFromPACSJob.data(), SIGNAL(PACSJobFinished(PACSJobPointer)), SLOT(retrieveDICOMFilesFromPACSJobFinished(PACSJobPointer)));
    connect(retrieveDICOMFilesFromPACSJob.data(), SIGNAL(PACSJobCancelled(PACSJobPointer)), SLOT(retrieveDICOMFilesFromPACSJobCancelled(PACSJobPointer)));

    PacsManagerSingleton::instance()->enqueuePACSJob(retrieveDICOMFilesFromPACSJob);

    return retrieveDICOMFilesFromPACSJob;
}

void RISRequestManager::retrieveStudyFromDatabase(Study *study)
{
    m_qpopUpRISRequestsScreen->addStudyRetrievedFromDatabaseByAccessionNumber(study);

    //Ara mateix no cal descarregar un estudi de la base de dades si ja li tenim, per això invoquem directament el mètode doActionsAfterRetrieve
    doActionsAfterRetrieve(study);
}

void RISRequestManager::retrieveDICOMFilesFromPACSJobCancelled(PACSJobPointer pacsJob)
{
    QSharedPointer<RetrieveDICOMFilesFromPACSJob> retrieveDICOMFilesFromPACSJob = pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>();

    if (retrieveDICOMFilesFromPACSJob.isNull())
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

/// Slot que s'activa quan un job de descarrega d'una petició del RIS ha finalitzat
void RISRequestManager::retrieveDICOMFilesFromPACSJobFinished(PACSJobPointer pacsJob)
{
    QSharedPointer<RetrieveDICOMFilesFromPACSJob> retrieveDICOMFilesFromPACSJob = pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>();

    if (retrieveDICOMFilesFromPACSJob.isNull())
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un RetrieveDICOMFilesFromPACSJob");
        return;
    }

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

    doActionsAfterRetrieve(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles());
}

void RISRequestManager::doActionsAfterRetrieve(Study *study)
{
    //Les descarregues d'altres peticions que no siguin l'actual les ignorem. (Per exemple cas en que el metge primer demana descarregar uns estudis i llavors se n'adona
    //que no era aquells que volia, doncs els estudis descarregats de la primera petició s'ignoraran i no se'n farà res)
    if (Settings().getValue(InputOutputSettings::RISRequestViewOnceRetrieved).toBool() && study->getAccessionNumber() == m_accessionNumberOfLastRequest)
    {
        if (!m_signalViewStudyEmittedForLastRISRequest)
        {
            m_signalViewStudyEmittedForLastRISRequest = true;
            emit viewStudyRetrievedFromRISRequest(study->getInstanceUID());
        }
        else
        {
            //Si ja s'ha emés un view per veure un estudi d'aquesta petició, pels altres de la petició emetrem un load
            emit loadStudyRetrievedFromRISRequest(study->getInstanceUID());
        }
    }
}

RISRequestManager::DICOMSourcesFromRetrieveStudy RISRequestManager::getDICOMSouceFromRetrieveStudy(Study *study)
{
    DICOMSourcesFromRetrieveStudy DICOMSourceFromRetrieveStudy;

    if (LocalDatabaseManager().studyExists(study->getInstanceUID()))
    {
        if (!m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeenRetrievedAgain)
        {
            INFO_LOG("S'han trobat estudis sol.licitats pel RIS a la base de dades local, es preguntara l'usuari si vol tornar-los a descarregar del PACS");
            m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeenRetrievedAgain = true;
            m_studiesInDatabaseHaveToBeenRetrievedAgain = askToUserIfRetrieveFromPACSStudyWhenExistsInDatabase(study->getParentPatient()->getFullName());

            if (m_studiesInDatabaseHaveToBeenRetrievedAgain)
            {
                INFO_LOG("L'usuari ha indicat que s'han de tornar a descarregar del PACS els estudis sol.licitats del RIS trobats a la base de dades local");
            }
            else
            {
                INFO_LOG("L'usuari ha indicat que no s'han de tornar a descarregar del PACS els estudis sol.licitats del RIS trobats a la base de dades local");
            }
        }

        DICOMSourceFromRetrieveStudy = m_studiesInDatabaseHaveToBeenRetrievedAgain ? PACS : Database;
    }
    else
    {
        DICOMSourceFromRetrieveStudy = PACS;
    }

    return DICOMSourceFromRetrieveStudy;
}

bool RISRequestManager::askToUserIfRetrieveFromPACSStudyWhenExistsInDatabase(const QString &fullPatientName) const
{
    QMessageBoxAutoClose qmessageBoxAutoClose(secondsTimeOutToHidePopUpAndAutoCloseQMessageBox);

    //Necessari indicar que estigui a sobre de tots els elements perquè sinó es mostra a sota del QPopUpRISRequestScreen impedint que l'usuari pugui llegir-ne el contingut
    //Hack: El SetWindowFlags s'ha de fer abans de setButtonToShowAutoCloseTimer, ja que sinó el botó que mostra el comptador endarrera no queda seleccionat per defecte
    qmessageBoxAutoClose.setWindowFlags(qmessageBoxAutoClose.windowFlags() | Qt::WindowStaysOnTopHint);
    qmessageBoxAutoClose.setText(tr("Some studies of patient %1 requested from RIS exist in the local database. Do you want to retrieve them again?") .arg(fullPatientName));
    qmessageBoxAutoClose.setWindowTitle(ApplicationNameString);
    qmessageBoxAutoClose.setIcon(QMessageBox::Question);
    qmessageBoxAutoClose.addButton(QMessageBox::Yes);
    QPushButton *pushButtonNo = qmessageBoxAutoClose.addButton(QMessageBox::No);
    qmessageBoxAutoClose.setButtonToShowAutoCloseTimer(pushButtonNo);

    qmessageBoxAutoClose.exec();

    return (qobject_cast<QPushButton*>(qmessageBoxAutoClose.clickedButton())) != pushButtonNo;
}

void RISRequestManager::showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error)
{
    QString message;
    int risPort = Settings().getValue(InputOutputSettings::RISRequestsPort).toInt();

    switch (error)
    {
        case ListenRISRequests::RisPortInUse:
            message = tr("Unable to listen to RIS requests on port %1, the port is in use by another application.").arg(risPort);
            break;
        case ListenRISRequests::UnknownNetworkError:
            message = tr("Unable to listen to RIS requests on port %1, an unknown network error has occurred.").arg(risPort);
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            break;
    }

    QMessageBox::critical(NULL, ApplicationNameString, message);
}

}
