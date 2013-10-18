#ifndef PETVOLUMEDISPLAYUNITHANDLER_H
#define PETVOLUMEDISPLAYUNITHANDLER_H

#include "singlevolumedisplayunithandler.h"

namespace udg {

/**
    VolumeDisplayUnit handler to handle a single PET input.
*/
class PETVolumeDisplayUnitHandler : public SingleVolumeDisplayUnitHandler {

public:
    PETVolumeDisplayUnitHandler();
    virtual ~PETVolumeDisplayUnitHandler();

};

}

#endif
