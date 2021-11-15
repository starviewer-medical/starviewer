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

#include "studyoperationsservice.h"

#include "dimsequerystudyoperationresult.h"
#include "dimseretrievestudyoperationresult.h"
#include "dimsestorestudyoperationresult.h"
#include "pacsdevice.h"
#include "pacsmanager.h"
#include "querypacsjob.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "senddicomfilestopacsjob.h"
#include "study.h"

namespace udg {

StudyOperationResult* StudyOperationsService::searchPacs(const PacsDevice &pacs, const DicomMask &mask, TargetResource targetResource)
{
    if (pacs.getType() == PacsDevice::Type::Dimse)
    {
        QueryPacsJob::QueryLevel queryLevel = QueryPacsJob::study;

        switch (targetResource) {
            case TargetResource::Studies:
                queryLevel = QueryPacsJob::study;
                break;
            case TargetResource::Series:
                queryLevel = QueryPacsJob::series;
                break;
            case TargetResource::Instances:
                queryLevel = QueryPacsJob::image;
                break;
        }

        PACSJobPointer job(new QueryPacsJob(pacs, mask, queryLevel));
        StudyOperationResult *result = new DimseQueryStudyOperationResult(job, m_pacsManager);

        // TODO connects from result to this

        m_pacsManager->enqueuePACSJob(job);

        emit operationRequested(result);

        return result;
    }

    return nullptr;
}

StudyOperationResult* StudyOperationsService::retrieveFromPacs(const PacsDevice &pacs, const Study *study, const QString &seriesInstanceUid,
                                                               const QString &sopInstanceUid, RetrievePriority priority)
{
    if (pacs.getType() == PacsDevice::Type::Dimse)
    {
        RetrieveDICOMFilesFromPACSJob::RetrievePriorityJob jobPriority;

        switch (priority)
        {
            case RetrievePriority::Low:
                jobPriority = RetrieveDICOMFilesFromPACSJob::Low;
                break;
            case RetrievePriority::Medium:
                jobPriority = RetrieveDICOMFilesFromPACSJob::Medium;
                break;
            case RetrievePriority::High:
                jobPriority = RetrieveDICOMFilesFromPACSJob::High;
                break;
        }

        PACSJobPointer job(new RetrieveDICOMFilesFromPACSJob(pacs, jobPriority, study, seriesInstanceUid, sopInstanceUid));
        StudyOperationResult *result = new DimseRetrieveStudyOperationResult(job, m_pacsManager);

        // TODO connects from result to this

        m_pacsManager->enqueuePACSJob(job);

        emit operationRequested(result);

        return result;
    }

    return nullptr;
}

StudyOperationResult* StudyOperationsService::storeInPacs(const PacsDevice &pacs, const Study *study)
{
    return storeInPacs(pacs, study->getSeries());
}

StudyOperationResult* StudyOperationsService::storeInPacs(const PacsDevice &pacs, const QList<Series*> &seriesList)
{
    if (pacs.getType() == PacsDevice::Type::Dimse)
    {
        QList<Image*> imageList;

        for (Series *series : seriesList)
        {
            imageList.append(series->getImages());
        }

        // TODO SendDICOMFilesToPACSJob and its internal SendDICOMFilesToPACS can only send images. They can't send encapsulated documents, nor presentation
        // states nor any other kind of instance. This should be addressed some time.
        PACSJobPointer job(new SendDICOMFilesToPACSJob(pacs, imageList));
        StudyOperationResult *result = new DimseStoreStudyOperationResult(job, m_pacsManager);

        // TODO connects from result to this

        m_pacsManager->enqueuePACSJob(job);

        emit operationRequested(result);

        return result;
    }

    return nullptr;
}

StudyOperationsService::StudyOperationsService(QObject *parent)
    : QObject(parent), m_pacsManager(nullptr)
{
    m_pacsManager = new PacsManager();
}

} // namespace udg
