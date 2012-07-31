#ifndef TESTINGVOLUME_H
#define TESTINGVOLUME_H

#include "volume.h"

using namespace udg;

namespace testing {

class TestingVolume : public udg::Volume {

public:

    TestingVolume(QObject *parent = 0);

public:

    /// El VolumeReader que retornarà createVolumeReader().
    VolumeReader *m_volumeReaderToUse;

private:

    virtual VolumeReader* createVolumeReader();

};

}

#endif // TESTINGVOLUME_H
