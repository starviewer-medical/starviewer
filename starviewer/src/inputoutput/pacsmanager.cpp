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

#include "pacsmanager.h"

#include <QThread>
#include <QTime>

#include "dicommask.h"
#include "pacsdevice.h"
#include "logging.h"
#include "querypacsjob.h"
#include "pacsjob.h"
#include "inputoutputsettings.h"

namespace udg {

/// Classe utilitza per adormir el Thread al mètode waitForAllPACSJobsFinished, m'entre s'espera que hagin finalitzat totes les operacions.
class Sleeper : public QThread {
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

PacsManager::PacsManager()
{
    Settings settings;

    m_queryQueue = NULL;
    m_queryQueue = new ThreadWeaver::Queue();
    m_queryQueue->setMaximumNumberOfThreads(settings.getValue(InputOutputSettings::MaximumPACSConnections).toInt());

    m_sendDICOMFilesToPACSQueue = new ThreadWeaver::Queue();
    m_sendDICOMFilesToPACSQueue->setMaximumNumberOfThreads(settings.getValue(InputOutputSettings::MaximumPACSConnections).toInt());

    m_retrieveDICOMFilesFromPACSQueue = new ThreadWeaver::Queue();
    // Només podem descarregar un estudi a la vegada del PACS, per això com a número màxim de threads especifiquem 1
    m_retrieveDICOMFilesFromPACSQueue ->setMaximumNumberOfThreads(1);
}

void PacsManager::enqueuePACSJob(PACSJobPointer pacsJob)
{
    pacsJob->setSelfPointer(pacsJob);

    switch (pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            m_sendDICOMFilesToPACSQueue->enqueue(pacsJob);
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            m_retrieveDICOMFilesFromPACSQueue->enqueue(pacsJob);
            break;
        case PACSJob::QueryPACS:
            m_queryQueue->enqueue(pacsJob);
            break;
        default:
            ERROR_LOG("Tipus de job invalid");
            break;
    }

    emit newPACSJobEnqueued(pacsJob);
}

// TODO: S'hauria de convertir al plural
bool PacsManager::isExecutingPACSJob()
{
    return !m_sendDICOMFilesToPACSQueue->isIdle() || !m_retrieveDICOMFilesFromPACSQueue->isIdle() || !m_queryQueue->isIdle();
}

bool PacsManager::isExecutingPACSJob(PACSJob::PACSJobType pacsJobType)
{
    switch (pacsJobType)
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            return !m_sendDICOMFilesToPACSQueue->isIdle();
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            return !m_retrieveDICOMFilesFromPACSQueue->isIdle();
            break;
        case PACSJob::QueryPACS:
            return !m_queryQueue->isIdle();
            break;
        default:
            ERROR_LOG("Metode isExecutingPACS ha rebut un Tipus de job invalid");
            return false;
    }
}

void PacsManager::requestCancelPACSJob(PACSJobPointer pacsJob)
{
    // El emit de requestedCancelPACSJob s'ha de fer abans de desencuar i requestAbort perquè sinó ens podem trobar que primer rebem el signal del PACSJob
    // PACSJobCancelledi llavors el requestedCancelPACSJob

    bool pacsJobIsExecuting;

    emit requestedCancelPACSJob(pacsJob);

    switch (pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            pacsJobIsExecuting = !m_sendDICOMFilesToPACSQueue->dequeue(pacsJob);
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            pacsJobIsExecuting = !m_retrieveDICOMFilesFromPACSQueue->dequeue(pacsJob);
            break;
        case PACSJob::QueryPACS:
            pacsJobIsExecuting = !m_queryQueue->dequeue(pacsJob);
            break;
        default:
            ERROR_LOG("Metode requestCancel ha rebut un Tipus de job invalid");
            return;
    }

    if (pacsJobIsExecuting)
    {
        // Si no l'hem pogut desencuar vol dir que s'està executant demanem abortar el job
        pacsJob->requestAbort();
    }
}

void PacsManager::requestCancelAllPACSJobs()
{
    m_sendDICOMFilesToPACSQueue->dequeue();
    m_sendDICOMFilesToPACSQueue->requestAbort();
    m_retrieveDICOMFilesFromPACSQueue->dequeue();
    m_retrieveDICOMFilesFromPACSQueue->requestAbort();
    m_queryQueue->dequeue();
    m_queryQueue->requestAbort();
}

bool PacsManager::waitForAllPACSJobsFinished(int msec)
{
    if (!isExecutingPACSJob())
    {
        return true;
    }

    QTime timer;
    timer.start();

    while (isExecutingPACSJob() && timer.elapsed() < msec)
    {
        Sleeper().msleep(50);
    }

    return !isExecutingPACSJob();
}

}; // End udg namespace
