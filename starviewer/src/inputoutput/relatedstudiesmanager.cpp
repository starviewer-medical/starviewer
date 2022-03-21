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

#include "dicommask.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "pacsdevicemanager.h"
#include "patient.h"
#include "settings.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"
#include "volumehelper.h"

namespace udg {

RelatedStudiesManager::RelatedStudiesManager()
{
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
    INFO_LOG(QString("Searching studies of patient with ID %1.").arg(patient->getID()));

    this->makeAsynchronousStudiesQuery(patient);
}

void RelatedStudiesManager::queryMergedPreviousStudies(Study *study)
{
    INFO_LOG(QString("Searching previous studies of study %1 from date %2, of patient with ID %3.")
             .arg(study->getInstanceUID(), study->getDate().toString(), study->getParentPatient()->getID()));

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
                                                                                              StudyOperations::TargetResource::Studies);
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
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);

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

void RelatedStudiesManager::onStudyRetrieveStarted(StudyOperationResult *result)
{
    emit studyRetrieveStarted(result->getRequestStudyInstanceUid());
}

void RelatedStudiesManager::onStudyRetrieveFinished(StudyOperationResult *result)
{
    emit studyRetrieveFinished(result->getRequestStudyInstanceUid());
    // Now that the study is downloaded and saved in the database this should go through the first branch
    Study *study = new Study();
    study->setInstanceUID(result->getRequestStudyInstanceUid());
    loadStudy(study);
    delete study;
}

void RelatedStudiesManager::onStudyRetrieveFailed(StudyOperationResult *result)
{
    emit studyRetrieveFailed(result->getRequestStudyInstanceUid());
}

void RelatedStudiesManager::onStudyRetrieveCancelled(StudyOperationResult *result)
{
    emit studyRetrieveCancelled(result->getRequestStudyInstanceUid());
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
    StudyOperationResult *result = StudyOperationsService::instance()->retrieveFromPacs(pacsDevice, study);

    // These connections will be deleted when result is destroyed
    connect(result, &StudyOperationResult::started, this, &RelatedStudiesManager::onStudyRetrieveStarted);
    connect(result, &StudyOperationResult::finishedSuccessfully, this, &RelatedStudiesManager::onStudyRetrieveFinished);
    connect(result, &StudyOperationResult::finishedWithPartialSuccess, this, &RelatedStudiesManager::onStudyRetrieveFinished);
    connect(result, &StudyOperationResult::finishedWithError, this, &RelatedStudiesManager::onStudyRetrieveFailed);
    connect(result, &StudyOperationResult::cancelled, this, &RelatedStudiesManager::onStudyRetrieveCancelled);
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);
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
