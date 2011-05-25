#include "asynchronousvolumereader.h"

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/DebuggingAids.h>

#include "volumereaderjob.h"
#include "volume.h"
#include "logging.h"

namespace udg {

QHash<int, VolumeReaderJob*> AsynchronousVolumeReader::m_volumesLoading;

AsynchronousVolumeReader::AsynchronousVolumeReader(QObject *parent)
    : QObject(parent)
{
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
    connect(volumeReaderJob, SIGNAL(done(ThreadWeaver::Job *)), SLOT(unmarkVolumeFromJobAsLoading(ThreadWeaver::Job *)));

    this->markVolumeAsLoadingByJob(volume, volumeReaderJob);

    // TODO Permetre escollir quants jobs alhora volem
    ThreadWeaver::Weaver *weaver = this->getWeaverInstance();
    weaver->enqueue(volumeReaderJob);

    return volumeReaderJob;
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
            connect(job, SIGNAL(done(ThreadWeaver::Job *)), volume, SLOT(deleteLater()));
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
