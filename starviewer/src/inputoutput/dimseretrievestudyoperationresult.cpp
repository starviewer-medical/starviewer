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

    // Slot will be executed in the same thread that executes the job (checked)
    connect(m_job.data(), &PACSJob::PACSJobFinished, this, &DimseRetrieveStudyOperationResult::onJobFinished, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobCancelled, this, &DimseRetrieveStudyOperationResult::onJobCancelled, Qt::DirectConnection);
}

QString DimseRetrieveStudyOperationResult::getSeriesInstanceUid() const
{
    return QString();
}

void DimseRetrieveStudyOperationResult::cancel()
{
    m_pacsManager->requestCancelPACSJob(m_job);
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
        onJobCancelled();   // TODO not sure if really needed
    }
    else
    {
        setErrorText(m_job->getStatusDescription());
    }
}

// TODO can a job be cancelled externally? maybe on application exit?
void DimseRetrieveStudyOperationResult::onJobCancelled()
{
    setCancelled();
}

} // namespace udg
