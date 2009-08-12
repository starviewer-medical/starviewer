#include "pacsmanager.h"

#include "dicommask.h"
#include "pacsdevice.h"
#include "logging.h"
#include "querypacsjob.h"

namespace udg{

PacsManager::PacsManager()
{
    m_queryWeaver = NULL;
    m_queryWeaver = new Weaver();
    m_queryWeaver->setMaximumNumberOfThreads(PacsDevice::getMaximumConnections());

    connect ( m_queryWeaver,  SIGNAL ( jobDone ( ThreadWeaver::Job* ) ), SLOT ( queryJobFinished ( ThreadWeaver::Job* ) ) );
}

void PacsManager::queryStudy(DicomMask mask, QList<PacsDevice> pacsToQuery)
{
    foreach(PacsDevice pacs, pacsToQuery)
    {
        QueryPacsJob *queryPacsJob = new QueryPacsJob(pacs, mask, QueryPacsJob::study);

        m_queryWeaver->enqueue ( queryPacsJob );
    }
}

void PacsManager::querySeries(DicomMask mask, PacsDevice pacsToQuery)
{
    QueryPacsJob *queryPacsJob = new QueryPacsJob(pacsToQuery, mask, QueryPacsJob::series);

    m_queryWeaver->enqueue ( queryPacsJob );
}

void PacsManager::queryImage(DicomMask mask, PacsDevice pacsToQuery)
{
    QueryPacsJob *queryPacsJob = new QueryPacsJob(pacsToQuery, mask, QueryPacsJob::image);

    m_queryWeaver->enqueue ( queryPacsJob );
}

void PacsManager::queryJobFinished ( ThreadWeaver::Job* job )
{
    QueryPacsJob *queryPacsJob = dynamic_cast<QueryPacsJob*> ( job );
    QString studyInstanceUID, seriesInstanceUID;

    //Encara haguem abortat el job, se'ns indica quan ha finalitzat per això comprovem si ha estat abortat per ignora els resultats que ens envia
    if (!queryPacsJob->isAbortRequested())
    {
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
                studyInstanceUID = queryPacsJob->getDicomMask().getStudyUID();
    
                if (queryPacsJob->getStatus().good())
                {
                    emit querySeriesResultsReceived(studyInstanceUID, queryPacsJob->getSeriesList());
                }
                else emit errorQueryingSeries(studyInstanceUID, queryPacsJob->getPacsDevice()); 
                break;
            case QueryPacsJob::image :
                studyInstanceUID = queryPacsJob->getDicomMask().getStudyUID();
                seriesInstanceUID = queryPacsJob->getDicomMask().getSeriesUID();
    
                if (queryPacsJob->getStatus().good())
                {
                    emit queryImageResultsReceived(studyInstanceUID, seriesInstanceUID, queryPacsJob->getImageList());
                }
                else emit errorQueryingImage(studyInstanceUID, seriesInstanceUID, queryPacsJob->getPacsDevice());
    
                break;
        }
    }

    if (m_queryWeaver->isIdle()) //Si no s'estan executant més consultes indiquem a finalitzat la cerca
    {
        emit queryFinished();
    }

    delete queryPacsJob;
}

void PacsManager::cancelCurrentQueries()
{
    m_queryWeaver->dequeue(); //Desencuem les consultes pendents de realitzar, el requestAbort només cancel.la les que s'estan executant
    m_queryWeaver->requestAbort();
}

bool PacsManager::isExecutingQueries()
{
    return !m_queryWeaver->isIdle();
}

}; //end udg namespace
