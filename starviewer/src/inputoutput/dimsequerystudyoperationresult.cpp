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

#include "dimsequerystudyoperationresult.h"

#include "logging.h"
#include "pacsmanager.h"
#include "querypacsjob.h"

namespace udg {

DimseQueryStudyOperationResult::DimseQueryStudyOperationResult(PACSJobPointer job, PacsManager *pacsManager, QObject *parent)
    : StudyOperationResult(parent), m_pacsManager(pacsManager)
{
    m_job = job.objectCast<QueryPacsJob>();

    if (m_job.isNull())
    {
        ERROR_LOG("DimseQueryStudyOperationResult created with a job that is not a QueryPacsJob");
        Q_ASSERT(false);    // fail in debug; just log and return in release
        return;
    }

    m_requestPacsDevice = m_job->getPacsDevice();

    switch (m_job->getQueryLevel())
    {
        case QueryPacsJob::study:
            m_requestLevel = RequestLevel::Studies;
            break;
        case QueryPacsJob::series:
            m_requestLevel = RequestLevel::Series;
            break;
        case QueryPacsJob::image:
            m_requestLevel = RequestLevel::Instances;
            break;
    }

    m_requestStudyInstanceUid = m_job->getDicomMask().getStudyInstanceUID();
    m_requestSeriesInstanceUid = m_job->getDicomMask().getSeriesInstanceUID();
    m_requestSopInstanceUid = m_job->getDicomMask().getSOPInstanceUID();

    // Slot will be executed in the same thread that executes the job (checked)
    connect(m_job.data(), &PACSJob::PACSJobStarted, this, &DimseQueryStudyOperationResult::onJobStarted, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobFinished, this, &DimseQueryStudyOperationResult::onJobFinished, Qt::DirectConnection);
    connect(m_job.data(), &PACSJob::PACSJobCancelled, this, &DimseQueryStudyOperationResult::onJobCancelled, Qt::DirectConnection);
}

StudyOperationResult::OperationType DimseQueryStudyOperationResult::getOperationType() const
{
    return OperationType::Search;
}

void DimseQueryStudyOperationResult::cancel()
{
    m_pacsManager->requestCancelPACSJob(m_job);
}

void DimseQueryStudyOperationResult::onJobStarted()
{
    emit started(this);
}

void DimseQueryStudyOperationResult::onJobFinished()
{
    if (m_job->getStatus() == PACSRequestStatus::QueryOk)
    {
        switch (m_requestLevel)
        {
            case RequestLevel::Studies:
                setStudies(m_job->getPatientStudyList());
                break;
            case RequestLevel::Series:
                setSeries(m_job->getSeriesList());
                break;
            case RequestLevel::Instances:
                setInstances(m_job->getImageList());
                break;
        }
    }
    else if (m_job->getStatus() == PACSRequestStatus::QueryCancelled)
    {
        onJobCancelled();
    }
    else
    {
        setErrorText(m_job->getStatusDescription());
    }
}

void DimseQueryStudyOperationResult::onJobCancelled()
{
    setCancelled();
}

} // namespace udg
