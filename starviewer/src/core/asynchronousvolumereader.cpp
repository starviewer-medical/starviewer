#include "asynchronousvolumereader.h"

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/DebuggingAids.h>

#include "volumereaderjob.h"
#include "volume.h"
#include "logging.h"
#include "coresettings.h"
#include "volumerepository.h"
#include "image.h"
#include "series.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace udg {

QHash<int, VolumeReaderJob*> AsynchronousVolumeReader::m_volumesLoading;
ThreadWeaver::ResourceRestrictionPolicy AsynchronousVolumeReader::m_resourceRestrictionPolicy;

AsynchronousVolumeReader::AsynchronousVolumeReader(QObject *parent)
    : QObject(parent)
{
}

bool is32BitWindows()
{
#if defined(_WIN64)
    return false;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    BOOL f64 = false;
    return !(IsWow64Process(GetCurrentProcess(), &f64) && f64);
#else
    return false; // Win64 does not support Win16
#endif
}

VolumeReaderJob* AsynchronousVolumeReader::read(Volume *volume)
{
    DEBUG_LOG(QString("AsynchronousVolumeReader::read Begin volume: %1").arg(volume->getIdentifier().getValue()));

    if (this->isVolumeLoading(volume))
    {
        DEBUG_LOG(QString("AsynchronousVolumeReader::read Volume already loading: %1").arg(volume->getIdentifier().getValue()));

        return this->getVolumeReaderJob(volume);
    }

    VolumeReaderJob *volumeReaderJob = new VolumeReaderJob(volume);
    assignResourceRestrictionPolicy(volumeReaderJob);

    connect(volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), SLOT(unmarkVolumeFromJobAsLoading(ThreadWeaver::Job*)));

    this->markVolumeAsLoadingByJob(volume, volumeReaderJob);

    // TODO Permetre escollir quants jobs alhora volem
    ThreadWeaver::Weaver *weaver = this->getWeaverInstance();
    weaver->enqueue(volumeReaderJob);

    return volumeReaderJob;
}

void AsynchronousVolumeReader::assignResourceRestrictionPolicy(VolumeReaderJob *volumeReaderJob)
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
    else if (is32BitWindows())
    {
        // Si és 32 bits limitem la concurrència si tenim volums multiframe o que no siguin CT o MR
        QStringList allowedModalities;
        allowedModalities << QString("CT") << QString("MR");
        bool found = false;
        QListIterator<Volume*> iterator(VolumeRepository::getRepository()->getItems());
        while (iterator.hasNext() && !found)
        {
            Volume *currentVolume = iterator.next();
            QList<Image*> imageList = currentVolume->getImages();
            // Mirem si és multiframe
            if (imageList.count() > 1)
            {
                // Comprovant la primera i segona imatges n'hi ha prou
                if (imageList.at(0)->getPath() == imageList.at(1)->getPath())
                {
                    found = true;
                }
            }
            // Mirem si és una modalitat permesa
            if (imageList.count() > 0)
            {
                QString modality = imageList.at(0)->getParentSeries()->getModality();
                if (!allowedModalities.contains(modality))
                {
                    found = true;
                }
            }
        }

        int numberOfVolumesLoadingConcurrently;
        if (found)
        {
            numberOfVolumesLoadingConcurrently = 1;
            INFO_LOG(QString("Windows 32 bits amb volums que poden requerir molta memòria. Limitem a %1 la quantitat de volums carregant-se simultàniament.").arg(numberOfVolumesLoadingConcurrently));
        }
        else
        {
            numberOfVolumesLoadingConcurrently = getWeaverInstance()->maximumNumberOfThreads();
        }
        m_resourceRestrictionPolicy.setCap(numberOfVolumesLoadingConcurrently);
        volumeReaderJob->assignQueuePolicy(&m_resourceRestrictionPolicy);
    }
}

void AsynchronousVolumeReader::unmarkVolumeFromJobAsLoading(ThreadWeaver::Job *job)
{
    // TODO Aquí és el lloc més correcte per desmarcar el volume?? Així tenim el problema de que no podem destruïr aquest objecte
    // fins que s'ha finalitzat el job, si no, no es marcaria mai com a carregat. Si no es fa aquí, hem de tenir en compte
    // problemes de concurrència.
    VolumeReaderJob *volumeReaderJob = dynamic_cast<VolumeReaderJob*>(job);
    if (volumeReaderJob)
    {
        this->unmarkVolumeAsLoading(volumeReaderJob->getVolume());
    }
}

bool AsynchronousVolumeReader::isVolumeLoading(Volume *volume) const
{
    return m_volumesLoading.contains(volume->getIdentifier().getValue());
}

void AsynchronousVolumeReader::cancelLoadingAndDeleteVolume(Volume *volume)
{
    if (this->isVolumeLoading(volume))
    {
        DEBUG_LOG(QString("Volume %1 isLoading, trying dequeue").arg(volume->getIdentifier().getValue()));
        VolumeReaderJob *job = this->getVolumeReaderJob(volume);
        ThreadWeaver::Weaver *weaver = this->getWeaverInstance();
        if (weaver->dequeue(job))
        {
            delete volume;
        }
        else
        {
            DEBUG_LOG(QString("Volume %1 cannot be dequeued, requesting abort and delete").arg(volume->getIdentifier().getValue()));
            connect(job, SIGNAL(done(ThreadWeaver::Job*)), volume, SLOT(deleteLater()));
            job->requestAbort();
        }
    }
    else
    {
        delete volume;
    }
}

void AsynchronousVolumeReader::markVolumeAsLoadingByJob(Volume *volume, VolumeReaderJob *volumeReaderJob)
{
    DEBUG_LOG(QString("markVolumeAsLoading: Volume %1").arg(volume->getIdentifier().getValue()));
    m_volumesLoading.insert(volume->getIdentifier().getValue(), volumeReaderJob);
}

void AsynchronousVolumeReader::unmarkVolumeAsLoading(Volume *volume)
{
    DEBUG_LOG(QString("unmarkVolumeAsLoading: Volume %1").arg(volume->getIdentifier().getValue()));
    m_volumesLoading.remove(volume->getIdentifier().getValue());
}

ThreadWeaver::Weaver* AsynchronousVolumeReader::getWeaverInstance() const
{
    // TODO De moment es retorna la instància global, caldria permetre passar-la com a paràmetre.
    return ThreadWeaver::Weaver::instance();
}

VolumeReaderJob* AsynchronousVolumeReader::getVolumeReaderJob(Volume *volume) const
{
    if (this->isVolumeLoading(volume))
    {
        return m_volumesLoading.value(volume->getIdentifier().getValue());
    }
    else
    {
        return NULL;
    }
}

} // End namespace udg
