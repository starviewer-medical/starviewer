#ifndef UDGSINGLEVOLUMEDISPLAYUNITHANDLER_H
#define UDGSINGLEVOLUMEDISPLAYUNITHANDLER_H

#include "genericvolumedisplayunithandler.h"

namespace udg {

/**
    VolumeDisplayUnit handler to handle a single input.
    Maximum number of inputs will be one.
*/
class SingleVolumeDisplayUnitHandler : public GenericVolumeDisplayUnitHandler {
public:
    SingleVolumeDisplayUnitHandler();
    ~SingleVolumeDisplayUnitHandler();

    int getMaximumNumberOfInputs() const;
};

} // End namespace udg

#endif
