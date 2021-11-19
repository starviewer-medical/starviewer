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

#include "externalstudyrequestmanager.h"

#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "pacsdevicemanager.h"
#include "patient.h"
#include "qmessageboxautoclose.h"
#include "qpopupexternalstudyrequestsscreen.h"
#include "starviewerapplication.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"
#include "usermessage.h"

#include <QMessageBox>

namespace udg {

namespace {

// Time in seconds until the pop-up notification is hidden after all downloads have finished. Also time until the the auto-closing message box is closed.
// Both must be the same because it could happen that first a study is downloaded from PACS and then a second study is found in the database; in this case the
// user must have time to choose an option in the message box, thus the notification must not be closed before the message box.
// Or at least this is what I understood from the original comment.
constexpr int SecondsTimeOutToHidePopUpAndAutoCloseQMessageBox = 5;

// Returns a translated string with the used query parameter in the given mask, to be used with logs and message boxes.
QString getQueryParameterString(const DicomMask &mask)
{
    if (!mask.getAccessionNumber().isEmpty())
    {
        return QObject::tr("Accession Number %1").arg(mask.getAccessionNumber());
    }
    else
    {
        return QObject::tr("Study Instance UID %1").arg(mask.getStudyInstanceUID());
    }
}

// Returns a string to be used in logs to identify the given PACS.
QString getPacsIdentificationString(const PacsDevice &pacs)
{
    QString id = QString("[ID %1] ").arg(pacs.getID());

    if (!pacs.getDescription().isEmpty())
    {
        return id + pacs.getDescription();
    }
    else if (pacs.getType() == PacsDevice::Type::Dimse)
    {
        return id + pacs.getAETitle();
    }
    else
    {
        return id + pacs.getBaseUri().toDisplayString();
    }
}

}

ExternalStudyRequestManager::ExternalStudyRequestManager(QObject *parent)
    : QObject(parent), m_listenRISRequests(nullptr)
{
    m_qpopUpExternalStudyRequestsScreen = new QPopUpExternalStudyRequestsScreen();
    m_qpopUpExternalStudyRequestsScreen->setTimeOutToHidePopUpAfterStudiesHaveBeenRetrieved(SecondsTimeOutToHidePopUpAndAutoCloseQMessageBox * 1000);
}

ExternalStudyRequestManager::~ExternalStudyRequestManager()
{
    delete m_qpopUpExternalStudyRequestsScreen;

    if (m_listenRISRequests)
    {
        // Com que la classe que escolta les peticions del RIS s'executa en un thread li emetem un signal indicant-li que s'ha de parar.
        emit stopListenRISRequests();

        m_listenRISRequestsQThread->exit();
        m_listenRISRequestsQThread->wait();

        m_listenRISRequests->deleteLater();

        delete m_listenRISRequestsQThread;
    }
}

void ExternalStudyRequestManager::initializeListener()
{
    m_listenRISRequestsQThread = new QThread();
    m_listenRISRequests = new ListenRISRequests();

    // La classe ListenRISRequests necessita el seu propi thread perquè sempre està executant-se esperant noves peticions, si l'executes el thread principal
    // Starviewer quedaria congelada només escoltant peticions del RIS
    m_listenRISRequests->moveToThread(m_listenRISRequestsQThread);
    m_listenRISRequestsQThread->start();

    createConnections();
}

void ExternalStudyRequestManager::createConnections()
{
    connect(m_listenRISRequests, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(processRequest(DicomMask)));
    connect(m_listenRISRequests, SIGNAL(errorListening(ListenRISRequests::ListenRISRequestsError)),
            SLOT(showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError)));
    // Hem d'indica a la classe ListenRISRequests que pot començar a escoltar/parar peticions a través d'un signal, perquè si ho fèssim invocant el mètode
    // listen() o stopListen directament aquest seria executat pel thread que l'invoca i podria fer petar l'aplicaciño, en canvi amb un signal aquest és
    // atés pel thread al que pertany ListenRISRequests
    connect(this, SIGNAL(listenRISRequests()), m_listenRISRequests, SLOT(listen()));
    connect(this, SIGNAL(stopListenRISRequests()), m_listenRISRequests, SLOT(stopListen()));
}

void ExternalStudyRequestManager::listen()
{
    if (Settings().getValue(InputOutputSettings::ListenToRISRequests).toBool())
    {
        initializeListener();
        emit listenRISRequests();
    }
}

void ExternalStudyRequestManager::processRequest(DicomMask dicomMaskRISRequest)
{
    INFO_LOG(QString("[SAP/RIS/command line] Queuing request for study with %1.").arg(getQueryParameterString(dicomMaskRISRequest)));

    // Per anar atenent les descàrregues a mesura que ens arriben encuem les peticions, només fem una cerca al PACS a la vegada, una
    // vegada hem trobat l'estudi en algun PACS, es posa a descarregar i s'aten una altra petició
    m_requestsQueue.enqueue(dicomMaskRISRequest);
    m_signalViewStudyEmittedForLastRequest = false;

    // Si tenim més d'un element ja hi ha un altre consulta d'un RIS Executant-se per tant no fem res
    if (m_requestsQueue.count() == 1)
    {
        queryPacsForRequest(m_requestsQueue.head());
    }
}

void ExternalStudyRequestManager::queryPacsForRequest(DicomMask maskRISRequest)
{
    INFO_LOG(QString("[SAP/RIS/command line] Starting query for study with %1.").arg(getQueryParameterString(maskRISRequest)));

    m_numberOfStudiesAddedToRetrieveForCurrentRequest = 0;
    // Al iniciar una nova consulta netegem la llista UID d'estudis demanats per descarregar i pendents de descarregar
    // TODO:Si ens arriba una altre petició del RIS mentre encara descarreguem l'anterior petició no es farà seguiment de la descarrega actual, sinó de la
    // última que ha arribat
    m_studiesInstancesUIDRequestedToRetrieve.clear();

    m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeRetrievedAgain = false;
    m_studiesInDatabaseHaveToBeRetrievedAgain = false;

    // TODO Ara mateix cal que nosaltres mateixos fem aquesta comprovació però potser seria interessant que el mètode PACSDevicemanager::queryStudy()
    // fes aquesta comprovació i ens retornes algun codi que pugui descriure com ha anat la consulta i així poder actuar en conseqüència mostrant
    // un message box, fent un log o el que calgui segons la ocasió.
    QList<PacsDevice> queryablePACS =
            PacsDeviceManager::getPacsList(PacsDeviceManager::DimseWithQueryRetrieveService | PacsDeviceManager::Wado | PacsDeviceManager::OnlyDefault);
    if (queryablePACS.isEmpty())
    {
        QMessageBox::information(nullptr, ApplicationNameString, tr("Cannot retrieve the studies requested from SAP, RIS or command line because there is no "
                                                                    "configured default PACS to query.\n\nPlease, check your PACS settings."));
        INFO_LOG("[SAP/RIS/command line] Can't process request because there are no default PACS.");
        m_requestsQueue.dequeue();
        return;
    }

    // Mostrem el popUP amb l'accession number
    m_qpopUpExternalStudyRequestsScreen->queryStudiesStarted();
    m_qpopUpExternalStudyRequestsScreen->activateWindow();
    m_qpopUpExternalStudyRequestsScreen->show();

    foreach (const PacsDevice &pacsDevice, queryablePACS)
    {
        StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacsDevice, maskRISRequest, StudyOperations::TargetResource::Studies);
        addPendingQuery(result);
    }
}

void ExternalStudyRequestManager::addPendingQuery(StudyOperationResult *result)
{
    // These connections will be deleted when result is destroyed
    connect(result, &StudyOperationResult::finishedSuccessfully, this, &ExternalStudyRequestManager::addFoundStudiesToRetrieveQueue);
    connect(result, &StudyOperationResult::finishedWithError, this, &ExternalStudyRequestManager::onQueryError);
    connect(result, &StudyOperationResult::cancelled, this, &ExternalStudyRequestManager::onQueryCancelled);
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);

    m_pendingQueryResults.insert(result);
}

void ExternalStudyRequestManager::addFoundStudiesToRetrieveQueue(StudyOperationResult *result)
{
    for (Patient *patient : result->getStudies())
    {
        for (Study *study : patient->getStudies())
        {
            if (!m_studiesInstancesUIDRequestedToRetrieve.contains(study->getInstanceUID()))
            {
                INFO_LOG(QString("[SAP/RIS/command line] Found a study that matches the search criterion. Study Instance UID %1. PACS %2")
                         .arg(study->getInstanceUID()).arg(getPacsIdentificationString(study->getDICOMSource().getRetrievePACS().at(0))));

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
                WARN_LOG(QString("[SAP/RIS/command line] Found study with Study Instance UID %1 in PACS %2 that matches the search criterion but it is already "
                                 "being retrieved from another PACS.")
                         .arg(study->getInstanceUID()).arg(getPacsIdentificationString(study->getDICOMSource().getRetrievePACS().at(0))));
            }
        }
    }

    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        onQueryFinished();
    }
}

void ExternalStudyRequestManager::onQueryError(StudyOperationResult *result)
{
    ERROR_LOG(QString("[SAP/RIS/command line] Request error: %1").arg(result->getErrorText()));
    QMessageBox::critical(nullptr, ApplicationNameString, tr("SAP, RIS or command line request error: %1").arg(result->getErrorText()));

    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        onQueryFinished();
    }
}

void ExternalStudyRequestManager::onQueryCancelled(StudyOperationResult *result)
{
    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        onQueryFinished();
    }
}

void ExternalStudyRequestManager::onQueryFinished()
{
    DicomMask dicomMaskRISRequest = m_requestsQueue.dequeue();
    INFO_LOG(QString("[SAP/RIS/command line] Query for study with %1 has finished.").arg(getQueryParameterString(dicomMaskRISRequest)));

    if (m_numberOfStudiesAddedToRetrieveForCurrentRequest == 0)
    {
        INFO_LOG(QString("[SAP/RIS/command line] No study found with %1.").arg(getQueryParameterString(dicomMaskRISRequest)));

        // Si no hem trobat cap estudi que coincideix llancem MessageBox
        QString message = tr("Unable to execute the SAP, RIS or command line request. The study with %1 was not found in the default PACS.")
                .arg(getQueryParameterString(dicomMaskRISRequest));

        m_qpopUpExternalStudyRequestsScreen->showNotStudiesFoundMessage();
        QMessageBox::information(nullptr, ApplicationNameString, message);
    }

    if (m_requestsQueue.count() > 0)
    {
        INFO_LOG("[SAP/RIS/command line] There are more requests pending to execute.");
        // Tenim altres sol·licituds del RIS per descarregar, les processem
        queryPacsForRequest(m_requestsQueue.head());
    }
}

void ExternalStudyRequestManager::retrieveFoundStudiesInQueue()
{
    while (!m_studiesToRetrieveQueue.isEmpty())
    {
        retrieveStudyFoundInQueryPACS(m_studiesToRetrieveQueue.head());

        //Al esborra els pacients automàticament s'esborren els Study del Patient. Study i Patient hereden de QObject i els objectes Study estan inserits com a fills QObject de Patient.
        //Això fa que quan eliminem Patient s'esborrin els estudis, perquè tot objecte que heredi de QObject quan es destrueix fa un delete dels seus QObject fills, que en aquest cas seria Study
        m_studiesToRetrieveQueue.dequeue()->getParentPatient()->deleteLater();
    }
}

void ExternalStudyRequestManager::retrieveStudyFoundInQueryPACS(Study *study)
{
    switch (getDICOMSouceFromRetrieveStudy(study))
    {
        case PACS:
            INFO_LOG(QString("[SAP/RIS/command line] Study %1 will be retrieved from PACS.").arg(study->getInstanceUID()));
            retrieveStudyFromPACS(study);
            break;
        
        case Database:
            INFO_LOG(QString("[SAP/RIS/command line] Study %1 will be retrieved from local database.").arg(study->getInstanceUID()));
            retrieveStudyFromDatabase(study);
            break;
    }

    m_numberOfStudiesAddedToRetrieveForCurrentRequest++;
}

void ExternalStudyRequestManager::retrieveStudyFromPACS(Study *study)
{
    PacsDevice pacsDevice = study->getDICOMSource().getRetrievePACS().at(0);

    StudyOperationResult *result = StudyOperationsService::instance()->retrieveFromPacs(pacsDevice, study);

    m_qpopUpExternalStudyRequestsScreen->addStudyToRetrieveFromPacs(result);
    connect(result, &StudyOperationResult::finishedSuccessfully, this, &ExternalStudyRequestManager::onStudyRetrieveSucceeded);
    connect(result, &StudyOperationResult::finishedWithPartialSuccess, this, &ExternalStudyRequestManager::onStudyRetrieveFinishedWithPartialSuccess);
    connect(result, &StudyOperationResult::finishedWithError, this, &ExternalStudyRequestManager::onStudyRetrieveFailed);
    connect(result, &StudyOperationResult::cancelled, this, &ExternalStudyRequestManager::onStudyRetrieveCancelled);
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);
}

void ExternalStudyRequestManager::retrieveStudyFromDatabase(Study *study)
{
    m_qpopUpExternalStudyRequestsScreen->addStudyRetrievedFromDatabase(study);

    //Ara mateix no cal descarregar un estudi de la base de dades si ja li tenim, per això invoquem directament el mètode doActionsAfterRetrieve
    doActionsAfterRetrieve(study->getInstanceUID());
}

void ExternalStudyRequestManager::onStudyRetrieveSucceeded(StudyOperationResult *result)
{
    doActionsAfterRetrieve(result->getRequestStudyInstanceUid());
}

void ExternalStudyRequestManager::onStudyRetrieveFinishedWithPartialSuccess(StudyOperationResult *result)
{
    QMessageBox::warning(nullptr, ApplicationNameString, result->getErrorText());
    onStudyRetrieveSucceeded(result);
}

void ExternalStudyRequestManager::onStudyRetrieveFailed(StudyOperationResult *result)
{
    QMessageBox::critical(nullptr, ApplicationNameString, result->getErrorText());
}

void ExternalStudyRequestManager::onStudyRetrieveCancelled(StudyOperationResult *result)
{
    INFO_LOG(QString("[SAP/RIS/command line] Download of study %1 from PACS %2 has been cancelled.").arg(result->getRequestStudyInstanceUid())
             .arg(getPacsIdentificationString(result->getRequestPacsDevice())));
}

void ExternalStudyRequestManager::doActionsAfterRetrieve(const QString &studyInstanceUid)
{
    //Les descarregues d'altres peticions que no siguin l'actual les ignorem. (Per exemple cas en que el metge primer demana descarregar uns estudis i llavors se n'adona
    //que no era aquells que volia, doncs els estudis descarregats de la primera petició s'ignoraran i no se'n farà res)
    if (Settings().getValue(InputOutputSettings::RISRequestViewOnceRetrieved).toBool() && m_studiesInstancesUIDRequestedToRetrieve.contains(studyInstanceUid))
    {
        if (!m_signalViewStudyEmittedForLastRequest)
        {
            m_signalViewStudyEmittedForLastRequest = true;
            emit viewStudyRetrievedFromRequest(studyInstanceUid);
        }
        else
        {
            //Si ja s'ha emés un view per veure un estudi d'aquesta petició, pels altres de la petició emetrem un load
            emit loadStudyRetrievedFromRequest(studyInstanceUid);
        }
    }
}

ExternalStudyRequestManager::DICOMSourcesFromRetrieveStudy ExternalStudyRequestManager::getDICOMSouceFromRetrieveStudy(Study *study)
{
    DICOMSourcesFromRetrieveStudy DICOMSourceFromRetrieveStudy;

    if (LocalDatabaseManager().studyExists(study->getInstanceUID()))
    {
        if (!m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeRetrievedAgain)
        {
            INFO_LOG("[SAP/RIS/command line] Some requested studies have been found in the local database. "
                     "The user will be asked if they should be downloaded again from PACS.");
            m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeRetrievedAgain = true;
            m_studiesInDatabaseHaveToBeRetrievedAgain = askToUserIfRetrieveFromPACSStudyWhenExistsInDatabase(study->getParentPatient()->getFullName());

            if (m_studiesInDatabaseHaveToBeRetrievedAgain)
            {
                INFO_LOG("[SAP/RIS/command line] The user has decided to download again from PACS the studies found in the local database.");
            }
            else
            {
                INFO_LOG("[SAP/RIS/command line] The user has decided to not download again from PACS the studies found in the local database.");
            }
        }

        DICOMSourceFromRetrieveStudy = m_studiesInDatabaseHaveToBeRetrievedAgain ? PACS : Database;
    }
    else
    {
        DICOMSourceFromRetrieveStudy = PACS;
    }

    return DICOMSourceFromRetrieveStudy;
}

bool ExternalStudyRequestManager::askToUserIfRetrieveFromPACSStudyWhenExistsInDatabase(const QString &fullPatientName) const
{
    QMessageBoxAutoClose qmessageBoxAutoClose(SecondsTimeOutToHidePopUpAndAutoCloseQMessageBox);

    //Necessari indicar que estigui a sobre de tots els elements perquè sinó es mostra a sota del QPopUpRISRequestScreen impedint que l'usuari pugui llegir-ne el contingut
    //Hack: El SetWindowFlags s'ha de fer abans de setButtonToShowAutoCloseTimer, ja que sinó el botó que mostra el comptador endarrera no queda seleccionat per defecte
    qmessageBoxAutoClose.setWindowFlags(qmessageBoxAutoClose.windowFlags() | Qt::WindowStaysOnTopHint);
    QString question = tr("Some studies of patient %1 requested from SAP, RIS or command line exist in the local database. Do you want to retrieve them again?")
            .arg(fullPatientName);
    qmessageBoxAutoClose.setText(question);
    qmessageBoxAutoClose.setWindowTitle(ApplicationNameString);
    qmessageBoxAutoClose.setIcon(QMessageBox::Question);
    qmessageBoxAutoClose.addButton(QMessageBox::Yes);
    QPushButton *pushButtonNo = qmessageBoxAutoClose.addButton(QMessageBox::No);
    qmessageBoxAutoClose.setButtonToShowAutoCloseTimer(pushButtonNo);

    qmessageBoxAutoClose.exec();

    return (qobject_cast<QPushButton*>(qmessageBoxAutoClose.clickedButton())) != pushButtonNo;
}

void ExternalStudyRequestManager::showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error)
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

    QMessageBox::critical(nullptr, ApplicationNameString, message);
}

}
