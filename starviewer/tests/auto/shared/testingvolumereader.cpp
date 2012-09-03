#include "testingvolumereader.h"

#include "volume.h"

namespace testing {

TestingVolumeReader::TestingVolumeReader(bool &readHasBeenCalled, QObject *parent)
    : udg::VolumeReader(parent), m_readPixelData(0), m_readHasBeenCalled(readHasBeenCalled)
{
    m_readHasBeenCalled = false;
}

void TestingVolumeReader::read(Volume *volume)
{
    m_readHasBeenCalled = true;

    if (volume)
    {
        volume->setPixelData(m_readPixelData);
    }
}

}
