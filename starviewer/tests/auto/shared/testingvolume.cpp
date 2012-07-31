#include "testingvolume.h"

#include "testingvolumereader.h"

namespace testing {

TestingVolume::TestingVolume(QObject *parent)
    : udg::Volume(parent), m_volumeReaderToUse(0)
{
}

udg::VolumeReader* TestingVolume::createVolumeReader()
{
    return m_volumeReaderToUse;
}

}
