#include "testingvolumereader.h"

#include "volume.h"

namespace testing {

TestingVolumeReader::TestingVolumeReader(bool &readHasBeenCalled, QObject *parent)
    : udg::VolumeReader(parent), m_readHasBeenCalled(readHasBeenCalled)
{
    m_readHasBeenCalled = false;
}

void TestingVolumeReader::read(Volume *volume)
{
    Q_UNUSED(volume)

    m_readHasBeenCalled = true;
}

}
