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

#include "dimsestorestudyoperationresult.h"

#include "logging.h"
#include "pacsmanager.h"
#include "senddicomfilestopacsjob.h"
#include "study.h"

namespace udg {

DimseStoreStudyOperationResult::DimseStoreStudyOperationResult(PACSJobPointer job, PacsManager *manager, QObject *parent)
    : StudyOperationResult(parent), m_pacsManager(manager)
{
    m_job = job.objectCast<SendDICOMFilesToPACSJob>();

    if (m_job.isNull())
    {
        ERROR_LOG("DimseStoreStudyOperationResult created with a job that is not a SendDICOMFilesToPACSJob");
        Q_ASSERT(false);    // fail in debug; just log and return in release
        return;
    }

    m_requestPacsDevice = m_job->getPacsDevice();
    m_requestLevel = RequestLevel::Studies;
    m_requestStudyInstanceUid = m_job->getStudyOfDICOMFilesToSend()->getInstanceUID();
    m_requestStudy = m_job->getStudyOfDICOMFilesToSend();

    // Slot will be executed in the same thread that executes the job (checked)
    connect(m_job.data(), &PACSJob::PACSJobStarted, this, &DimseStoreStudyOperationResult::onJobStarted, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobFinished, this, &DimseStoreStudyOperationResult::onJobFinished, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobCancelled, this, &DimseStoreStudyOperationResult::onJobCancelled, Qt::DirectConnection);
    connect(m_job.data(), static_cast<void(SendDICOMFilesToPACSJob::*)(PACSJobPointer,int)>(&SendDICOMFilesToPACSJob::DICOMFileSent),
            [this](PACSJobPointer, int numberOfDICOMFilesSent) {
                emit instanceTransferred(this, numberOfDICOMFilesSent);
            }
    );
    connect(m_job.data(), &SendDICOMFilesToPACSJob::DICOMSeriesSent, [this](PACSJobPointer, int numberOfSeriesSent) {
        emit seriesTransferred(this, numberOfSeriesSent);
    });
}

StudyOperationResult::OperationType DimseStoreStudyOperationResult::getOperationType() const
{
    return OperationType::Store;
}

void DimseStoreStudyOperationResult::cancel()
{
    m_pacsManager->requestCancelPACSJob(m_job);
}

void DimseStoreStudyOperationResult::onJobStarted()
{
    emit started(this);
}

void DimseStoreStudyOperationResult::onJobFinished()
{
    if (m_job->getStatus() == PACSRequestStatus::SendOk)
    {
        setNothing();
    }
    else if (m_job->getStatus() == PACSRequestStatus::SendWarningForSomeImages || m_job->getStatus() == PACSRequestStatus::SendSomeDICOMFilesFailed)
    {
        setNothing(m_job->getStatusDescription());
    }
    else if (m_job->getStatus() == PACSRequestStatus::SendCancelled)
    {
        onJobCancelled();   // TODO not sure if really needed
    }
    else
    {
        setErrorText(m_job->getStatusDescription());
    }
}

// TODO can a job be cancelled externally? maybe on application exit?
void DimseStoreStudyOperationResult::onJobCancelled()
{
    setCancelled();
}

} // namespace udg
