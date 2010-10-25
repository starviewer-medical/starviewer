#include "pacsmanager.h"

#include "dicommask.h"
#include "pacsdevice.h"
#include "logging.h"
#include "querypacsjob.h"
#include "pacsjob.h"
#include "inputoutputsettings.h"

namespace udg{

PacsManager::PacsManager()
{
    Settings settings;

    m_queryWeaver = NULL;
    m_queryWeaver = new ThreadWeaver::Weaver();
    m_queryWeaver->setMaximumNumberOfThreads(settings.getValue(InputOutputSettings::MaximumPACSConnections).toInt());
    
    m_sendDICOMFilesToPACSWeaver = new ThreadWeaver::Weaver();
    m_sendDICOMFilesToPACSWeaver->setMaximumNumberOfThreads(settings.getValue(InputOutputSettings::MaximumPACSConnections).toInt());

    m_retrieveDICOMFilesFromPACSWeaver  = new ThreadWeaver::Weaver();
    //Només podem descarregar un estudi a la vegada del PACS, per això com a número màxim de threads especifiquem 1
    m_retrieveDICOMFilesFromPACSWeaver ->setMaximumNumberOfThreads(1);
}

void PacsManager::enqueuePACSJob(PACSJob *pacsJob)
{
    switch (pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType :
            m_sendDICOMFilesToPACSWeaver->enqueue(pacsJob);
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            m_retrieveDICOMFilesFromPACSWeaver->enqueue(pacsJob);
            break;
        case PACSJob::QueryPACS:
            m_queryWeaver->enqueue(pacsJob);
            break;
        default:
            ERROR_LOG("Tipus de job invalid");
            break;
    }

    emit newPACSJobEnqueued(pacsJob);
}

bool PacsManager::isExecutingPACSJob(PACSJob::PACSJobType pacsJobType)
{
    switch (pacsJobType)
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            return !m_sendDICOMFilesToPACSWeaver->isIdle();
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            return !m_retrieveDICOMFilesFromPACSWeaver->isIdle();
            break;
        case PACSJob::QueryPACS:
            return m_queryWeaver->isIdle();
            break;   
        default:
            ERROR_LOG("Metode isExecutingPACS ha rebut un Tipus de job invalid");
            return false;
    }
}

void PacsManager::requestCancelPACSJob(PACSJob *pacsJob)
{
    /*El emit de requestedCancelPACSJob s'ha de fer abans de desencuar i requestAbort perquè sinó ens podem trobar que primer rebem el signal del PACSJob
     PACSJobCancelledi llavors el requestedCancelPACSJob*/

    bool pacsJobIsExecuting;

    emit requestedCancelPACSJob(pacsJob);

    switch (pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            pacsJobIsExecuting = !m_sendDICOMFilesToPACSWeaver->dequeue(pacsJob);
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            pacsJobIsExecuting = !m_retrieveDICOMFilesFromPACSWeaver->dequeue(pacsJob);
            break;
        case PACSJob::QueryPACS:
            pacsJobIsExecuting = !m_queryWeaver->dequeue(pacsJob);
            break;
        default:
            ERROR_LOG("Metode requestCancel ha rebut un Tipus de job invalid");
            return;
    }

    if (pacsJobIsExecuting)
    {
        //Si no l'hem pogut desencuar vol dir que s'està executant demanem abortar el job
        pacsJob->requestAbort();
    }
}

}; //end udg namespace
