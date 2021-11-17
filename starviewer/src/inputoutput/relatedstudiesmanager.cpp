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

#include "relatedstudiesmanager.h"

#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "pacsdevicemanager.h"
#include "pacsmanager.h"
#include "patient.h"
#include "settings.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"
#include "volumehelper.h"

namespace udg {

RelatedStudiesManager::RelatedStudiesManager()
{
    m_pacsManager = new PacsManager();
    m_studyInstanceUIDOfStudyToFindRelated = "invalid";

    Settings settings;
    m_searchRelatedStudiesByName = settings.getValue(InputOutputSettings::SearchRelatedStudiesByName).toBool();
}

RelatedStudiesManager::~RelatedStudiesManager()
{
    cancelCurrentQuery();
    deleteQueryResults();
}

void RelatedStudiesManager::queryMergedStudies(Patient *patient)
{
    INFO_LOG("Es buscaran els estudis del pacient " + patient->getFullName() + " amb ID " + patient->getID());

    this->makeAsynchronousStudiesQuery(patient);
}

void RelatedStudiesManager::queryMergedPreviousStudies(Study *study)
{
    INFO_LOG("Es buscaran els estudis previs del pacient " + study->getParentPatient()->getFullName() + " amb ID " + study->getParentPatient()->getID() +
    " de l'estudi " + study->getInstanceUID() + " fet a la data " + study->getDate().toString());

    m_studyInstanceUIDOfStudyToFindRelated = study->getInstanceUID();

    this->makeAsynchronousStudiesQuery(study->getParentPatient(), study->getDate());
}

void RelatedStudiesManager::makeAsynchronousStudiesQuery(Patient *patient, QDate untilDate)
{
    initializeQuery();

    QList<PacsDevice> pacsDeviceListToQuery =
            PacsDeviceManager::getPacsList(PacsDeviceManager::DimseWithQueryRetrieveService | PacsDeviceManager::Wado | PacsDeviceManager::OnlyDefault);
    pacsDeviceListToQuery = PacsDeviceManager::removeDuplicatePacsFromList(pacsDeviceListToQuery + getPACSRetrievedStudiesOfPatient(patient));

    if (pacsDeviceListToQuery.count() == 0)
    {
        // Sinó hi ha cap PACS pel qual cercar per defecte fem l'emit del queryFinished
        queryFinished();
        return;
    }

    QList<DicomMask> queryDicomMasksList = getDicomMasks(patient);

    if (queryDicomMasksList.count() == 0)
    {
        // Sinó hi ha cap cconsulta a fer donem la cerca per finalitzada
        queryFinished();
    }
    else
    {
        // Si ens diuen que volen els study's fins una data, hem de marcar aquesta data en els dicomMasks
        if (untilDate.isValid())
        {
            foreach (DicomMask dicomMask, queryDicomMasksList)
            {
                dicomMask.setStudyDate(QDate(), untilDate);
            }
        }

        foreach (const PacsDevice &pacsDevice, pacsDeviceListToQuery)
        {
            foreach (DicomMask queryDicomMask, queryDicomMasksList)
            {
                StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacsDevice, queryDicomMask,
                                                                                              StudyOperationsService::TargetResource::Studies);
                addPendingQuery(result);
            }
        }
    }
}

QList<Study*> RelatedStudiesManager::getStudiesFromDatabase(Patient *patient)
{
    QList<DicomMask> queryDicomMasksList = getDicomMasks(patient);
    LocalDatabaseManager database;
    QHash<QString, Study*> studies;

    foreach (const DicomMask &dicomMask, queryDicomMasksList)
    {
        foreach(Patient *p, database.queryPatientsAndStudies(dicomMask))
        {
            foreach (Study *study, p->getStudies())
            {
                studies.insert(study->getInstanceUID(), study);
            }
        }
    }
    return studies.values();
}

QList<DicomMask> RelatedStudiesManager::getDicomMasks(Patient *patient)
{
    QList<DicomMask> queryDicomMasksList;

    if (!patient->getID().isEmpty())
    {
        DicomMask maskQueryByID = getBasicDicomMask();
        maskQueryByID.setPatientID(patient->getID());
        queryDicomMasksList << maskQueryByID;
    }

    if (m_searchRelatedStudiesByName && !patient->getFullName().isEmpty())
    {
        DicomMask maskQueryByName = getBasicDicomMask();
        maskQueryByName.setPatientName(patient->getFullName());
        queryDicomMasksList << maskQueryByName;
    }

    return queryDicomMasksList;
}

void RelatedStudiesManager::initializeQuery()
{
    cancelCurrentQuery();

    /// Fem neteja de consultes anteriors
    deleteQueryResults();
}

void RelatedStudiesManager::addPendingQuery(StudyOperationResult *result)
{
    // This connections will be deleted when result is destroyed
    connect(result, &StudyOperationResult::finishedSuccessfully, this, &RelatedStudiesManager::mergeStudiesFromQuery);
    connect(result, &StudyOperationResult::finishedWithError, this, &RelatedStudiesManager::onQueryError);
    connect(result, &StudyOperationResult::cancelled, this, &RelatedStudiesManager::onQueryCancelled);

    m_pendingQueryResults.insert(result);
}

void RelatedStudiesManager::cancelCurrentQuery()
{
    for (auto it = m_pendingQueryResults.begin(); it != m_pendingQueryResults.end(); )
    {
        (*it)->cancel();
        it = m_pendingQueryResults.erase(it);
    }

    m_studyInstanceUIDOfStudyToFindRelated = "invalid";
}

bool RelatedStudiesManager::isExecutingQueries()
{
    return !m_pendingQueryResults.empty();
}

void RelatedStudiesManager::mergeStudiesFromQuery(StudyOperationResult *result)
{
    for (Patient *patient : result->getStudies())
    {
        for (Study *study : patient->getStudies())
        {
            if (!isStudyInMergedStudyList(study) && !isMainStudy(study))
            {
                // Si l'estudi no està a llista ja d'estudis afegits i no és el mateix estudi pel qua ens han demanat el
                // previ l'afegim
                m_mergedStudyList.append(study);
            }
        }
    }

    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        queryFinished();
    }

    result->deleteLater();
}

void RelatedStudiesManager::onQueryError(StudyOperationResult *result)
{
    ERROR_LOG(QString("Error when searching for related studies: %1").arg(result->getErrorText()));

    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        queryFinished();
    }

    result->deleteLater();
}

void RelatedStudiesManager::onQueryCancelled(StudyOperationResult *result)
{
    m_pendingQueryResults.erase(result);

    if (m_pendingQueryResults.empty())
    {
        queryFinished();
    }

    result->deleteLater();
}

void RelatedStudiesManager::onStudyRetrieveStarted(void *requester, PACSJobPointer pacsJob)
{
    if (requester == this)
    {
        emit studyRetrieveStarted(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles()->getInstanceUID());
    }
}

void RelatedStudiesManager::onStudyRetrieveFinished(void *requester, PACSJobPointer pacsJob)
{
    if (requester == this)
    {
        emit studyRetrieveFinished(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles()->getInstanceUID());
        // Now that the study is downloaded and saved in the database this should go through the first branch
        loadStudy(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles());
    }
}

void RelatedStudiesManager::onStudyRetrieveFailed(void *requester, PACSJobPointer pacsJob)
{
    if (requester == this)
    {
        emit studyRetrieveFailed(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles()->getInstanceUID());
    }
}

void RelatedStudiesManager::onStudyRetrieveCancelled(void *requester, PACSJobPointer pacsJob)
{
    if (requester == this)
    {
        emit studyRetrieveCancelled(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles()->getInstanceUID());
    }
}

void RelatedStudiesManager::queryFinished()
{
    // Quan totes les query han acabat és quant fem l'emit amb els estudis previs trobats. No podem emetre els resultats que anem rebent,
    // perquè hem de fer un merge del resultats rebuts, per no tenir duplicats (Estudis del matiex pacient que estiguin a més d'un PACS)
    emit queryStudiesFinished(m_mergedStudyList);
}

bool RelatedStudiesManager::isStudyInMergedStudyList(Study *study)
{
    bool studyFoundInMergedList = false;

    foreach (Study *studyMerged, m_mergedStudyList)
    {
        if (study->getInstanceUID() == studyMerged->getInstanceUID())
        {
            studyFoundInMergedList = true;
            break;
        }
    }

    return studyFoundInMergedList;
}

bool RelatedStudiesManager::isMainStudy(Study *study)
{
    return study->getInstanceUID() == m_studyInstanceUIDOfStudyToFindRelated;
}

DicomMask RelatedStudiesManager::getBasicDicomMask()
{
    DicomMask dicomMask;

    /// Definim els camps que la consulta ha de retornar
    dicomMask.setPatientName("");
    dicomMask.setPatientID("");
    dicomMask.setStudyID("");
    dicomMask.setStudyDescription("");
    dicomMask.setStudyModality("");
    dicomMask.setStudyDate(QDate(), QDate());
    dicomMask.setStudyTime(QTime(), QTime());
    dicomMask.setStudyInstanceUID("");

    return dicomMask;
}

RelatedStudiesManager::LoadStatus RelatedStudiesManager::loadStudy(Study *study)
{
    LocalDatabaseManager localDatabaseManager;

    if (localDatabaseManager.studyExists(study->getInstanceUID()))
    {
        DicomMask mask;
        mask.setStudyInstanceUID(study->getInstanceUID());
        Patient *patient = localDatabaseManager.retrieve(mask);

        if (patient && patient->getNumberOfStudies() > 0)
        {
            VolumeHelper::generatePatientVolumes(patient);
            emit studyLoaded(patient->getStudies().first());
            return Loaded;
        }
        else
        {
            ERROR_LOG(QString("Error loading study %1 from database despite it existed a moment before.").arg(study->getInstanceUID()));
            return Failed;
        }
    }
    else if (study->getDICOMSource().getRetrievePACS().count() > 0)
    {
        retrieveAndLoad(study, study->getDICOMSource().getRetrievePACS().at(0));

        return Retrieving;
    }
    else
    {
        return Failed;
    }
}

void RelatedStudiesManager::retrieveAndLoad(Study *study, const PacsDevice &pacsDevice)
{
    // We defer the connections until the first request to avoid unnecessary connections before
    // We use private slots so that we can specify Qt::UniqueConnection to avoid duplicate connections
    // They will be automatically disconnected when this RelatedStudiesManager is destroyed
    connect(PacsManagerSingleton::instance(), &PacsManager::studyRetrieveStarted, this, &RelatedStudiesManager::onStudyRetrieveStarted, Qt::UniqueConnection);
    connect(PacsManagerSingleton::instance(), &PacsManager::studyRetrieveFinished, this, &RelatedStudiesManager::onStudyRetrieveFinished, Qt::UniqueConnection);
    connect(PacsManagerSingleton::instance(), &PacsManager::studyRetrieveFailed, this, &RelatedStudiesManager::onStudyRetrieveFailed, Qt::UniqueConnection);
    connect(PacsManagerSingleton::instance(), &PacsManager::studyRetrieveCancelled, this, &RelatedStudiesManager::onStudyRetrieveCancelled, Qt::UniqueConnection);

    PacsManagerSingleton::instance()->retrieveStudy(this, pacsDevice, RetrieveDICOMFilesFromPACSJob::Medium, study);
}

void RelatedStudiesManager::deleteQueryResults()
{
    QList<Patient*> patientsStudy;

    foreach (Study* study, m_mergedStudyList)
    {
        patientsStudy.append(study->getParentPatient());
    }

    qDeleteAll(m_mergedStudyList);
    qDeleteAll(patientsStudy);

    m_mergedStudyList.clear();
}

QList<PacsDevice> RelatedStudiesManager::getPACSRetrievedStudiesOfPatient(Patient *patient)
{
    QList<PacsDevice> pacsDeviceRetrievedStudies;

    foreach (Study *studyPatient, patient->getStudies())
    {
        pacsDeviceRetrievedStudies = PacsDeviceManager::removeDuplicatePacsFromList(pacsDeviceRetrievedStudies + studyPatient->getDICOMSource().getRetrievePACS());
    }

    return pacsDeviceRetrievedStudies;
}

}
