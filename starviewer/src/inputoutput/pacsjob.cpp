#include "pacsjob.h"

namespace udg
{

int PACSJob::m_jobIDCounter = 0;

PACSJob::PACSJob(PacsDevice pacsDevice)
{
    m_pacsDevice = pacsDevice;
    m_jobID = m_jobIDCounter++;

    //Ens connectem amb els signals de ThreadWeaver::Job per poder emtre els nostres propis signals quan un PACSJob s'ha començat a executar o ha finalitzat
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

void PACSJob::threadWeaverJobDone()
{
    emit PACSJobFinished(this);
}

void PACSJob::threadWeaverJobStarted()
{
    emit PACSJobStarted(this);
}

};