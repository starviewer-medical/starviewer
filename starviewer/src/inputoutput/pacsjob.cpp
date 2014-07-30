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

#include "pacsjob.h"

namespace udg {

namespace {

int PACSJobPointerMetaTypeId = qRegisterMetaType<PACSJobPointer>("PACSJobPointer");

}

int PACSJob::m_jobIDCounter = 0;

PACSJob::PACSJob(PacsDevice pacsDevice)
{
    m_pacsDevice = pacsDevice;
    m_jobID = m_jobIDCounter++;
    m_abortIsRequested = false;
}

PacsDevice PACSJob::getPacsDevice()
{
    return m_pacsDevice;
}

int PACSJob::getPACSJobID()
{
    return m_jobID;
}

void PACSJob::aboutToBeDequeued(ThreadWeaver::QueueAPI *)
{
    // Si ens desenqueuen de la cua de jobs pendents d'executar, vol dir que aquest Job no s'executarà, per tant emetem signal indicant que
    // ens han cancel·lat
    emit PACSJobCancelled(m_selfPointer.toStrongRef());
}

void PACSJob::requestAbort()
{
    m_abortIsRequested = true;
    requestCancelJob();
}

bool PACSJob::isAbortRequested()
{
    return m_abortIsRequested;
}

void PACSJob::setSelfPointer(const PACSJobPointer &self)
{
    m_selfPointer = self;
}

void PACSJob::defaultBegin(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(thread)
    emit PACSJobStarted(job.dynamicCast<PACSJob>());
}

void PACSJob::defaultEnd(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(thread)

    if (!m_abortIsRequested)
    {
        emit PACSJobFinished(job.dynamicCast<PACSJob>());
    }
    else
    {
        emit PACSJobCancelled(job.dynamicCast<PACSJob>());
    }
}

};
