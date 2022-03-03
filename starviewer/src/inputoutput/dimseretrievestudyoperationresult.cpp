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

#include "dimseretrievestudyoperationresult.h"

#include "logging.h"
#include "pacsmanager.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "study.h"

namespace udg {

DimseRetrieveStudyOperationResult::DimseRetrieveStudyOperationResult(PACSJobPointer job, PacsManager *pacsManager, QObject *parent)
    : StudyOperationResult(parent), m_pacsManager(pacsManager)
{
    m_job = job.objectCast<RetrieveDICOMFilesFromPACSJob>();

    if (m_job.isNull())
    {
        ERROR_LOG("DimseRetrieveStudyOperationResult created with a job that is not a RetrieveDICOMFilesFromPACSJob");
        Q_ASSERT(false);    // fail in debug; just log and return in release
        return;
    }

    m_requestPacsDevice = m_job->getPacsDevice();
    m_requestStudyInstanceUid = m_job->getStudyToRetrieveDICOMFiles()->getInstanceUID();
    m_requestSeriesInstanceUid = m_job->getSeriesInstanceUidToRetrieve();
    m_requestSopInstanceUid = m_job->getSopInstanceUidToRetrieve();
    m_requestStudy = m_job->getStudyToRetrieveDICOMFiles();

    if (!m_requestSopInstanceUid.isEmpty())
    {
        m_requestLevel = RequestLevel::Instances;
    }
    else if (!m_requestSeriesInstanceUid.isEmpty())
    {
        m_requestLevel = RequestLevel::Series;
    }
    else
    {
        m_requestLevel = RequestLevel::Studies;
    }

    // Slot will be executed in the same thread that executes the job (checked)
    connect(m_job.data(), &PACSJob::PACSJobStarted, this, &DimseRetrieveStudyOperationResult::onJobStarted, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobFinished, this, &DimseRetrieveStudyOperationResult::onJobFinished, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobCancelled, this, &DimseRetrieveStudyOperationResult::onJobCancelled, Qt::DirectConnection);
    connect(m_job.data(), static_cast<void(RetrieveDICOMFilesFromPACSJob::*)(PACSJobPointer,int)>(&RetrieveDICOMFilesFromPACSJob::DICOMFileRetrieved),
            [this](PACSJobPointer, int numberOfImagesRetrieved) {
                emit instanceTransferred(this, numberOfImagesRetrieved);
            }
    );
    connect(m_job.data(), &RetrieveDICOMFilesFromPACSJob::DICOMSeriesRetrieved, [this](PACSJobPointer, int numberOfSeriesRetrieved) {
        emit seriesTransferred(this, numberOfSeriesRetrieved);
    });
}

StudyOperationResult::OperationType DimseRetrieveStudyOperationResult::getOperationType() const
{
    return OperationType::Retrieve;
}

void DimseRetrieveStudyOperationResult::cancel()
{
    m_pacsManager->requestCancelPACSJob(m_job);
}

void DimseRetrieveStudyOperationResult::onJobStarted()
{
    emit started(this);
}

void DimseRetrieveStudyOperationResult::onJobFinished()
{
    if (m_job->getStatus() == PACSRequestStatus::RetrieveOk)
    {
        setStudyInstanceUid(m_job->getStudyToRetrieveDICOMFiles()->getInstanceUID());
    }
    else if (m_job->getStatus() == PACSRequestStatus::RetrieveSomeDICOMFilesFailed)
    {
        setStudyInstanceUid(m_job->getStudyToRetrieveDICOMFiles()->getInstanceUID(), m_job->getStatusDescription());
    }
    else if (m_job->getStatus() == PACSRequestStatus::RetrieveCancelled)
    {
        onJobCancelled();
    }
    else
    {
        setErrorText(m_job->getStatusDescription());
    }
}

void DimseRetrieveStudyOperationResult::onJobCancelled()
{
    setCancelled();
}

} // namespace udg
