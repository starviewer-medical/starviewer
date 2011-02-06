#include "volumereaderjob.h"

#include "volumereader.h"
#include "volume.h"
#include "logging.h"

namespace udg {

VolumeReaderJob::VolumeReaderJob(Volume *volume, QObject *parent)
    : Job(parent)
{
    m_volumeToRead = volume;
    m_volumeReadSuccessfully = false;
    m_lastErrorMessageToUser = "";
}

VolumeReaderJob::~VolumeReaderJob()
{
}

bool VolumeReaderJob::success() const
{
    return m_volumeReadSuccessfully;
}

QString VolumeReaderJob::getLastErrorMessageToUser() const
{
    return m_lastErrorMessageToUser;
}


Volume* VolumeReaderJob::getVolume() const
{
    return m_volumeToRead;
}


void VolumeReaderJob::run()
{
    Q_ASSERT(m_volumeToRead);

    DEBUG_LOG(QString("Begin run VolumeReaderJob with Volume: %1").arg(m_volumeToRead->getIdentifier().getValue()));

    VolumeReader *volumeReader = new VolumeReader();
    connect(volumeReader, SIGNAL(progress(int)), SIGNAL(progress(int)));
    m_volumeReadSuccessfully = volumeReader->readWithoutShowingError(m_volumeToRead);
    m_lastErrorMessageToUser = volumeReader->getLastErrorMessageToUser();

    DEBUG_LOG(QString("End run VolumeReaderJob with Volume: %1 and result %2").arg(m_volumeToRead->getIdentifier().getValue()).arg(m_volumeReadSuccessfully));
}

} // End namespace udg
