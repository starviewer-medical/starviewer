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

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

bool is32BitWindows()
{
#if defined(_WIN64)
    return false;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    BOOL f64 = false;
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    return !(fnIsWow64Process(GetCurrentProcess(), &f64) && f64);
#else
    return false; // Win64 does not support Win16
#endif
}

bool is32BitProgramOnWindows()
{
#if defined(_WIN64)
    return false;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    return true;
#else
    return false; // Win64 does not support Win16
#endif
}

}

namespace udg {

VolumeReaderJobFactory::VolumeReaderJobFactory(QObject *parent)
 : QObject(parent)
{
}

VolumeReaderJobFactory::~VolumeReaderJobFactory()
{
    m_volumesLoading.clear();

    this->getWeaverInstance()->dequeue();
    this->getWeaverInstance()->requestAbort();
    this->getWeaverInstance()->shutDown();

    // Since jobs' signals are connected through queued connections and they include shared pointers to the jobs itselfs, we need to make sure that any pending
    // signals are delivered to the corresponding receivers so that the jobs aren't kept alive in the signals queue after the factory and the resource
    // restriction policy have been deleted.
    QApplication::processEvents();

    DEBUG_LOG("VolumeReaderJobFactory is closed");
}

QSharedPointer<VolumeReaderJob> VolumeReaderJobFactory::read(Volume *volume)
{
    DEBUG_LOG(QString("AsynchronousVolumeReader::read Begin volume: %1").arg(volume->getIdentifier().getValue()));

    if (this->isVolumeLoading(volume))
    {
        DEBUG_LOG(QString("AsynchronousVolumeReader::read Volume already loading: %1").arg(volume->getIdentifier().getValue()));

        return this->getVolumeReaderJob(volume);
    }

    VolumeReaderJob *volumeReaderJob = new VolumeReaderJob(volume);
    QSharedPointer<VolumeReaderJob> jobPointer(volumeReaderJob);
    assignResourceRestrictionPolicy(volumeReaderJob);

    connect(volumeReaderJob, SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(unmarkVolumeFromJobAsLoading(ThreadWeaver::JobPointer)));

    this->markVolumeAsLoadingByJob(volume, jobPointer);

    // TODO Permetre escollir quants jobs alhora volem
    ThreadWeaver::Queue *queue = this->getWeaverInstance();
    queue->enqueue(jobPointer);

    return jobPointer;
}

void VolumeReaderJobFactory::assignResourceRestrictionPolicy(VolumeReaderJob *volumeReaderJob)
{
    QSettings settings;
    if (settings.contains(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently))
    {
        int maximumNumberOfVolumesLoadingConcurrently = Settings().getValue(CoreSettings::MaximumNumberOfVolumesLoadingConcurrently).toInt();
        if (maximumNumberOfVolumesLoadingConcurrently > 0)
        {
            m_resourceRestrictionPolicy.setCap(maximumNumberOfVolumesLoadingConcurrently);
            volumeReaderJob->assignQueuePolicy(&m_resourceRestrictionPolicy);
            INFO_LOG(QString("Limitem a %1 la quantitat de volums carregant-se simultàniament.").arg(m_resourceRestrictionPolicy.cap()));
        }
        else
        {
            ERROR_LOG("El valor per limitar la quantitat de volums carregant-se simultàniament ha de ser més gran de 0.");
        }
    }
    else 
    {
        QStringList allowedModalities;
        bool checkMultiframeImages = false;

        // If it's a 32 bit build, concurrence is disabled on multiframe volumes.
        // Moreover, if it's running on a 32bit Win, concurrence is only allowed for CT and MR volumes.
        if (is32BitProgramOnWindows())
        {
            checkMultiframeImages = true;

            if (is32BitWindows())
            {
                allowedModalities << QString("CT") << QString("MR");
            }
        }
        
        bool foundRestrictions = checkForResourceRestrictions(checkMultiframeImages, allowedModalities);
        
        int numberOfVolumesLoadingConcurrently;
        if (foundRestrictions)
        {
            numberOfVolumesLoadingConcurrently = 1;
            if (is32BitWindows())
            {
                INFO_LOG(QString("Windows 32 bits amb volums que poden requerir molta memòria. Limitem a %1 la quantitat de volums carregant-se simultàniament.")
                    .arg(numberOfVolumesLoadingConcurrently));
            }
            else
            {
                INFO_LOG(QString("Windows 64 bits amb volums multiframe que poden requerir molta memòria. Limitem a %1 la quantitat de volums carregant-se "
                    "simultàniament.").arg(numberOfVolumesLoadingConcurrently));
            }
        }
        else
        {
            numberOfVolumesLoadingConcurrently = getWeaverInstance()->maximumNumberOfThreads();
        }
        m_resourceRestrictionPolicy.setCap(numberOfVolumesLoadingConcurrently);
        volumeReaderJob->assignQueuePolicy(&m_resourceRestrictionPolicy);
    }
}

bool VolumeReaderJobFactory::checkForResourceRestrictions(bool checkMultiframeImages, const QStringList &modalitiesWithoutRestriction)
{
    if (!checkMultiframeImages && modalitiesWithoutRestriction.isEmpty())
    {
        return false;
    }
    
    bool foundRestriction = false;
    QListIterator<Volume*> iterator(VolumeRepository::getRepository()->getItems());
    while (iterator.hasNext() && !foundRestriction)
    {
        Volume *currentVolume = iterator.next();
        // Mirem si és multiframe
        if (checkMultiframeImages)
        {
            if (currentVolume->isMultiframe())
            {
                foundRestriction = true;
            }
        }

        // Mirem si és una modalitat a la que cal aplicar restricció o no
        if (!modalitiesWithoutRestriction.isEmpty())
        {
            QString modality = currentVolume->getModality();
            if (!modalitiesWithoutRestriction.contains(modality))
            {
                foundRestriction = true;
            }
        }
    }

    return foundRestriction;
}

void VolumeReaderJobFactory::unmarkVolumeFromJobAsLoading(ThreadWeaver::JobPointer job)
{
    // TODO Aquí és el lloc més correcte per desmarcar el volume?? Així tenim el problema de que no podem destruïr aquest objecte
    // fins que s'ha finalitzat el job, si no, no es marcaria mai com a carregat. Si no es fa aquí, hem de tenir en compte
    // problemes de concurrència.
    QSharedPointer<VolumeReaderJob> volumeReaderJob = job.dynamicCast<VolumeReaderJob>();
    if (volumeReaderJob)
    {
        this->unmarkVolumeAsLoading(volumeReaderJob->getVolumeIdentifier());
    }
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
        DEBUG_LOG(QString("Volume %1 isLoading, trying dequeue").arg(volume->getIdentifier().getValue()));
        ThreadWeaver::JobPointer job = this->getVolumeReaderJob(volume);
        ThreadWeaver::Queue *queue = this->getWeaverInstance();
        if (queue->dequeue(job))
        {
            delete volume;
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

void VolumeReaderJobFactory::markVolumeAsLoadingByJob(Volume *volume, QSharedPointer<VolumeReaderJob> volumeReaderJob)
{
    DEBUG_LOG(QString("markVolumeAsLoading: Volume %1").arg(volume->getIdentifier().getValue()));
    m_volumesLoading.insert(volume->getIdentifier().getValue(), volumeReaderJob);
}

void VolumeReaderJobFactory::unmarkVolumeAsLoading(const Identifier &volumeIdentifier)
{
    DEBUG_LOG(QString("unmarkVolumeAsLoading: Volume %1").arg(volumeIdentifier.getValue()));
    m_volumesLoading.remove(volumeIdentifier.getValue());
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
