#include "asynchronousvolumereader.h"

#include <threadweaver/ThreadWeaver.h>
#include <threadweaver/DebuggingAids.h>

#include "volumereaderjob.h"
#include "volume.h"

namespace udg {

QHash<int, VolumeReaderJob*> AsynchronousVolumeReader::m_volumesLoading;

AsynchronousVolumeReader::AsynchronousVolumeReader(QObject *parent)
    : QObject(parent)
{
}

VolumeReaderJob* AsynchronousVolumeReader::read(Volume *volume)
{
    DEBUG_LOG(QString("Read volume: %1").arg(volume->getIdentifier().getValue()));

    if (this->isVolumeLoading(volume))
    {
        DEBUG_LOG(QString("Volume already loading: %1").arg(volume->getIdentifier().getValue()));

        return m_volumesLoading.value(volume->getIdentifier().getValue());
    }

    VolumeReaderJob *volumeReaderJob = new VolumeReaderJob(volume);
    connect(volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), SLOT(unmarkVolumeFromJobAsLoading(ThreadWeaver::Job*)));

    this->markVolumeAsLoadingByJob(volume, volumeReaderJob);

    // TODO Permetre escollir quants jobs alhora volem
    ThreadWeaver::Weaver *weaver = ThreadWeaver::Weaver::instance();
    weaver->enqueue(volumeReaderJob);

    // Per activar debug de ThreadWeaver
    //ThreadWeaver::setDebugLevel(true, 4);

    return volumeReaderJob;
}

void AsynchronousVolumeReader::unmarkVolumeFromJobAsLoading(ThreadWeaver::Job* job)
{
    // TODO Aquí és el lloc més correcte per desmarcar el volume?? Així tenim el problema de que no podem destruïr aquest objecte
    // fins que s'ha finalitzat el job, si no, no es marcaria mai com a carregat. Si no es fa aquí, hem de tenir en compte
    // problemes de concurrència.
    VolumeReaderJob *volumeReaderJob = qobject_cast<VolumeReaderJob*>(job);
    if (volumeReaderJob)
    {
        this->unmarkVolumeAsLoading(volumeReaderJob->getVolume());
    }
}

bool AsynchronousVolumeReader::isVolumeLoading(Volume *volume) const
{
    return m_volumesLoading.contains(volume->getIdentifier().getValue());
}

void AsynchronousVolumeReader::markVolumeAsLoadingByJob(Volume *volume, VolumeReaderJob *volumeReaderJob)
{
    DEBUG_LOG(QString("markVolumeAsLoading: %1").arg(volume->getIdentifier().getValue()));
    m_volumesLoading.insert(volume->getIdentifier().getValue(), volumeReaderJob);
}

void AsynchronousVolumeReader::unmarkVolumeAsLoading(Volume *volume)
{
    DEBUG_LOG(QString("unmarkVolumeAsLoading:  %1").arg(volume->getIdentifier().getValue()));
    m_volumesLoading.remove(volume->getIdentifier().getValue());
}

} // End namespace udg
