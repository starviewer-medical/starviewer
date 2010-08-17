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
    m_numberOfQueryPacsJobsPending = 0;
    
    m_sendDICOMFilesToPACSWeaver = new ThreadWeaver::Weaver();
    m_sendDICOMFilesToPACSWeaver->setMaximumNumberOfThreads(settings.getValue(InputOutputSettings::MaximumPACSConnections).toInt());

    m_retrieveDICOMFilesFromPACSWeaver  = new ThreadWeaver::Weaver();
    //Només podem descarregar un estudi a la vegada del PACS, per això com a número màxim de threads especifiquem 1
    m_retrieveDICOMFilesFromPACSWeaver ->setMaximumNumberOfThreads(1);

    connect ( m_queryWeaver,  SIGNAL ( jobDone ( ThreadWeaver::Job* ) ), SLOT ( queryJobFinished ( ThreadWeaver::Job* ) ) );
}

void PacsManager::queryStudy(DicomMask mask, QList<PacsDevice> pacsToQuery)
{
    foreach(PacsDevice pacs, pacsToQuery)
    {
        QueryPacsJob *queryPacsJob = new QueryPacsJob(pacs, mask, QueryPacsJob::study);

        m_queryWeaver->enqueue ( queryPacsJob );
        m_numberOfQueryPacsJobsPending++;
    }
}

void PacsManager::querySeries(DicomMask mask, PacsDevice pacsToQuery)
{
    QueryPacsJob *queryPacsJob = new QueryPacsJob(pacsToQuery, mask, QueryPacsJob::series);

    m_queryWeaver->enqueue ( queryPacsJob );
    m_numberOfQueryPacsJobsPending++;
}

void PacsManager::queryImage(DicomMask mask, PacsDevice pacsToQuery)
{
    QueryPacsJob *queryPacsJob = new QueryPacsJob(pacsToQuery, mask, QueryPacsJob::image);

    m_queryWeaver->enqueue ( queryPacsJob );
    m_numberOfQueryPacsJobsPending++;
}

void PacsManager::queryJobFinished ( ThreadWeaver::Job* job )
{
    QueryPacsJob *queryPacsJob = dynamic_cast<QueryPacsJob*> ( job );
    QString studyInstanceUID, seriesInstanceUID;

    //Encara haguem abortat el job, se'ns indica quan ha finalitzat per això comprovem si ha estat abortat per ignora els resultats que ens envia
    if (!queryPacsJob->isAbortRequested())
    {
        m_numberOfQueryPacsJobsPending--;

        switch (queryPacsJob->getQueryLevel())
        {
            case QueryPacsJob::study :
                if (queryPacsJob->getStatus().good())
                {
                    emit queryStudyResultsReceived(queryPacsJob->getPatientStudyList(), queryPacsJob->getHashTablePacsIDOfStudyInstanceUID());
                }
                else emit errorQueryingStudy(queryPacsJob->getPacsDevice());
                break;
            case QueryPacsJob::series :
                studyInstanceUID = queryPacsJob->getDicomMask().getStudyInstanceUID();
    
                if (queryPacsJob->getStatus().good())
                {
                    emit querySeriesResultsReceived(studyInstanceUID, queryPacsJob->getSeriesList());
                }
                else emit errorQueryingSeries(studyInstanceUID, queryPacsJob->getPacsDevice()); 
                break;
            case QueryPacsJob::image :
                studyInstanceUID = queryPacsJob->getDicomMask().getStudyInstanceUID();
                seriesInstanceUID = queryPacsJob->getDicomMask().getSeriesInstanceUID();
    
                if (queryPacsJob->getStatus().good())
                {
                    emit queryImageResultsReceived(studyInstanceUID, seriesInstanceUID, queryPacsJob->getImageList());
                }
                else emit errorQueryingImage(studyInstanceUID, seriesInstanceUID, queryPacsJob->getPacsDevice());
    
                break;
        }
    }

    //TODO:No hauria d'anar dins el if de si el job no ha estat cancel·lat ?
    if (m_numberOfQueryPacsJobsPending == 0) //Si ja no tenim més jobs pendents d'atendre fem signal
    {
        emit queryFinished();
    }

    delete queryPacsJob;
}

void PacsManager::cancelCurrentQueries()
{
    m_queryWeaver->dequeue(); //Desencuem les consultes pendents de realitzar, el requestAbort només cancel.la les que s'estan executant
    m_queryWeaver->requestAbort();

    m_numberOfQueryPacsJobsPending = 0;
}

bool PacsManager::isExecutingQueries()
{
    return !m_queryWeaver->isIdle();
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
        default:
            ERROR_LOG("Metode isExecutingPACS ha rebut un Tipus de job invalid");
            return false;
    }
}

void PacsManager::requestCancelPACSJob(PACSJob *pacsJob)
{
    /*El emit de requestedCancelPACSJob s'ha de fer abans de desencuar i requestAbort perquè sinó ens podem trobar que primer rebem el signal del PACSJob
     PACSJobCancelledi llavors el requestedCancelPACSJob*/
    switch (pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            emit requestedCancelPACSJob(pacsJob);
            if (!m_sendDICOMFilesToPACSWeaver->dequeue(pacsJob))
            {
                //Si no l'hem pogut desencuar vol dir que s'està executant demanem abortar el job
                pacsJob->requestAbort();
            }
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            emit requestedCancelPACSJob(pacsJob);
            if (!m_retrieveDICOMFilesFromPACSWeaver->dequeue(pacsJob))
            {
                //Si no l'hem pogut desencuar vol dir que s'està executant demanem abortar el job
                pacsJob->requestAbort();
            }
            break;
        default:
            ERROR_LOG("Metode requestCancel ha rebut un Tipus de job invalid");
            return;
    }
}

}; //end udg namespace
