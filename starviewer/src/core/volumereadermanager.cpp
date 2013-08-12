#include "volumereadermanager.h"

#include "asynchronousvolumereader.h"
#include "volumereaderjob.h"
#include "volume.h"

namespace udg {

VolumeReaderManager::VolumeReaderManager(QObject *parent) :
    QObject(parent)
{
    m_volumeReaderJob = NULL;
}

void VolumeReaderManager::readVolume(Volume *volume)
{
    // TODO Esborrar volumeReader!!
    AsynchronousVolumeReader *volumeReader = new AsynchronousVolumeReader();
    m_volumeReaderJob = volumeReader->read(volume);
    connect(m_volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), SIGNAL(readingFinished()));
    connect(m_volumeReaderJob, SIGNAL(progress(int)), SIGNAL(progress(int)));
}

void VolumeReaderManager::cancelReading()
{
    // TODO: Aquí s'hauria de cancel·lar realment el current job. De moment no podem fer-ho i simplement el desconnectem
    // Quan es faci bé, tenir en compte què passa si algun altre visor el vol continuar descarregant igualment i nosaltres aquí el cancelem?
    if (!m_volumeReaderJob.isNull())
    {
        disconnect(m_volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), this, SIGNAL(readingFinished()));
        disconnect(m_volumeReaderJob, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
    }
    m_volumeReaderJob = NULL;
}

bool VolumeReaderManager::readingSuccess()
{
    return m_volumeReaderJob->success();
}

Volume* VolumeReaderManager::getVolume()
{
    return m_volumeReaderJob->getVolume();
}

QString VolumeReaderManager::getLastErrorMessageToUser()
{
    return m_volumeReaderJob->getLastErrorMessageToUser();
}

bool VolumeReaderManager::isReading()
{
    return !m_volumeReaderJob.isNull() && !m_volumeReaderJob->isFinished();
}

} // namespace udg
