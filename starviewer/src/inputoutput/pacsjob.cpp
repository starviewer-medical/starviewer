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

int PACSJob::m_jobIDCounter = 0;

PACSJob::PACSJob(PacsDevice pacsDevice)
{
    m_pacsDevice = pacsDevice;
    m_jobID = m_jobIDCounter++;
    m_abortIsRequested = false;

    // Ens connectem amb els signals de ThreadWeaver::Job per poder emtre els nostres propis signals quan un PACSJob s'ha començat a executar o ha finalitzat
    connect(this, SIGNAL(started(ThreadWeaver::Job*)), SLOT(threadWeaverJobStarted()));
    connect(this, SIGNAL(done(ThreadWeaver::Job*)), SLOT(threadWeaverJobDone()));
}

PacsDevice PACSJob::getPacsDevice()
{
    return m_pacsDevice;
}

int PACSJob::getPACSJobID()
{
    return m_jobID;
}

void PACSJob::aboutToBeDequeued(ThreadWeaver::WeaverInterface *)
{
    // Si ens desenqueuen de la cua de jobs pendents d'executar, vol dir que aquest Job no s'executarà, per tant emetem signal indicant que
    // ens han cancel·lat
    emit PACSJobCancelled(this);
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

void PACSJob::threadWeaverJobDone()
{
    if (!m_abortIsRequested)
    {
        emit PACSJobFinished(this);
    }
    else
    {
        emit PACSJobCancelled(this);
    }
}

void PACSJob::threadWeaverJobStarted()
{
    emit PACSJobStarted(this);
}

};
