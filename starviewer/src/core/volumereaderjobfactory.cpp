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

#include "volumereaderjobfactory.h"

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/DebuggingAids>
#include <ThreadWeaver/Queue>

#include "volumereaderjob.h"
#include "volume.h"
#include "logging.h"
#include "coresettings.h"
#include "volumerepository.h"
#include "image.h"
#include "series.h"

#include <QApplication>

namespace udg {

VolumeReaderJobFactory::VolumeReaderJobFactory(QObject *parent)
 : QObject(parent)
{
}

VolumeReaderJobFactory::~VolumeReaderJobFactory()
{
    m_volumesLoading.clear();
    m_volumeRequesters.clear();

    this->getWeaverInstance()->dequeue();
    this->getWeaverInstance()->requestAbort();
    this->getWeaverInstance()->shutDown();

    // Since jobs' signals are connected through queued connections and they include shared pointers to the jobs itselfs, we need to make sure that any pending
    // signals are delivered to the corresponding receivers so that the jobs aren't kept alive in the signals queue after the factory and the resource
    // restriction policy have been deleted.
    QApplication::processEvents();

    DEBUG_LOG("VolumeReaderJobFactory is closed");
}

void VolumeReaderJobFactory::read(void *requester, Volume *volume)
{
    int id = volume->getIdentifier().getValue();
    DEBUG_LOG(QString("Begin reading volume: %1").arg(id));

    if (this->isVolumeLoading(volume))
    {
        // If the volume is already loading we just add the new requester to the hash.
        // Since the connections are queued because the signals are emitted from another thread, there's no risk that the onJobDone slot is called before the
        // new requester is added; it can't be called until the next iteration of the event loop.
        DEBUG_LOG(QString("Volume already loading: %1").arg(id));
        m_volumeRequesters.insert(id, requester);
    }
    else
    {
        VolumeReaderJob *volumeReaderJob = new VolumeReaderJob(volume);
        QSharedPointer<VolumeReaderJob> jobPointer(volumeReaderJob);
        assignResourceRestrictionPolicy(volumeReaderJob);

        connect(volumeReaderJob, &VolumeReaderJob::progress, this, &VolumeReaderJobFactory::onJobProgress);
        connect(volumeReaderJob, &VolumeReaderJob::done, this, &VolumeReaderJobFactory::onJobDone);
        // These connections are undone when the job is destroyed

        m_volumesLoading.insert(id, jobPointer);
        m_volumeRequesters.insert(id, requester);

        ThreadWeaver::Queue *queue = this->getWeaverInstance();
        queue->enqueue(jobPointer);
    }
}

void VolumeReaderJobFactory::cancelRead(void *requester, Volume *volume)
{
    int id = volume->getIdentifier().getValue();
    DEBUG_LOG(QString("A requester is not interested anymore in reading volume: %1").arg(id));

    if (!this->isVolumeLoading(volume))
    {
        DEBUG_LOG(QString("The volume %1 is not loading. Maybe it has already finished or the caller is trolling me").arg(id));
        return;
    }

    m_volumeRequesters.remove(id, requester);

    if (!m_volumeRequesters.contains(id))
    {
        DEBUG_LOG(QString("No requesters left for volume %1").arg(id));
        // TODO Now it would be the time to call requestAbort() on the job, but the volume would be perpetually converted to a neutral volume and we don't want
        //      that. Until that is fixed, better do nothing.
        // m_volumesLoading[id]->requestAbort();
    }
}

void VolumeReaderJobFactory::assignResourceRestrictionPolicy(VolumeReaderJob *volumeReaderJob)
{
    Settings settings;
    int maximumNumberOfVolumesLoadingConcurrently = settings.getValue(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently).toInt();

    if (maximumNumberOfVolumesLoadingConcurrently <= 0)
    {
        WARN_LOG(QString("Invalid value in \"%1\" setting: %2. Resetting it to default.").arg(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently)
                                                                                         .arg(maximumNumberOfVolumesLoadingConcurrently));
        settings.remove(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently);
        maximumNumberOfVolumesLoadingConcurrently = settings.getValue(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently).toInt();
        DEBUG_LOG(QString("Default maximumNumberOfVolumesLoadingConcurrently: %1").arg(maximumNumberOfVolumesLoadingConcurrently));
    }

    m_resourceRestrictionPolicy.setCap(maximumNumberOfVolumesLoadingConcurrently);
    {
        QMutexLocker lock(volumeReaderJob->mutex());
        volumeReaderJob->assignQueuePolicy(&m_resourceRestrictionPolicy);
    }
    INFO_LOG(QString("Limitem a %1 la quantitat de volums carregant-se simultàniament.").arg(m_resourceRestrictionPolicy.cap()));
}

void VolumeReaderJobFactory::onJobProgress(ThreadWeaver::JobPointer job, int progress)
{
    VolumeReaderJob *volumeReaderJob = static_cast<VolumeReaderJob*>(job.get());
    int id = volumeReaderJob->getVolumeIdentifier().getValue();

    foreach (void *requester, m_volumeRequesters.values(id))
    {
        emit volumeReadingProgress(requester, volumeReaderJob->getVolume(), progress);
    }
}

void VolumeReaderJobFactory::onJobDone(ThreadWeaver::JobPointer job)
{
    VolumeReaderJob *volumeReaderJob = static_cast<VolumeReaderJob*>(job.get());
    int id = volumeReaderJob->getVolumeIdentifier().getValue();
    m_volumesLoading.remove(id);

    foreach (void *requester, m_volumeRequesters.values(id))
    {
        emit volumeReadingFinished(requester, volumeReaderJob);
    }

    m_volumeRequesters.remove(id);  // this removes all the items with id
}

bool VolumeReaderJobFactory::isVolumeLoading(Volume *volume) const
{
    if (!volume)
    {
        DEBUG_LOG("El volum és nul. No pot estar carregant-se.");
        return false;
    }

    return m_volumesLoading.contains(volume->getIdentifier().getValue());
}

void VolumeReaderJobFactory::cancelLoadingAndDeleteVolume(Volume *volume)
{
    if (!volume)
    {
        DEBUG_LOG("El volum és nul. No es pot fer cancel de la càrrega ni esborrar.");
        return;
    }

    if (this->isVolumeLoading(volume))
    {
        int id = volume->getIdentifier().getValue();
        DEBUG_LOG(QString("Volume %1 isLoading, trying dequeue").arg(id));
        ThreadWeaver::JobPointer job = this->getVolumeReaderJob(volume);
        ThreadWeaver::Queue *queue = this->getWeaverInstance();
        if (queue->dequeue(job))
        {
            delete volume;
            m_volumesLoading.remove(id);
            m_volumeRequesters.remove(id);
        }
        else
        {
            DEBUG_LOG(QString("Volume %1 cannot be dequeued, requesting abort and delete").arg(volume->getIdentifier().getValue()));
            // TODO This connection doesn't assure that the volume will be deleted,
            // because the job could finish and emit the signal before the connection is done.
            connect(job.staticCast<VolumeReaderJob>().data(), SIGNAL(done(ThreadWeaver::JobPointer)), volume, SLOT(deleteLater()));
            job->requestAbort();
        }
    }
    else
    {
        delete volume;
    }
}

ThreadWeaver::Queue* VolumeReaderJobFactory::getWeaverInstance() const
{
    // TODO De moment es retorna la instància global, caldria permetre passar-la com a paràmetre.
    return ThreadWeaver::Queue::instance();
}

QSharedPointer<VolumeReaderJob> VolumeReaderJobFactory::getVolumeReaderJob(Volume *volume) const
{
    if (this->isVolumeLoading(volume))
    {
        return m_volumesLoading.value(volume->getIdentifier().getValue());
    }
    else
    {
        return QSharedPointer<VolumeReaderJob>();
    }
}

} // End namespace udg
