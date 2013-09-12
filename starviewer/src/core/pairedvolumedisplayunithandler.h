#ifndef UDGPAIREDVOLUMEDISPLAYUNITHANDLER_H
#define UDGPAIREDVOLUMEDISPLAYUNITHANDLER_H

#include "genericvolumedisplayunithandler.h"

namespace udg {

/**
    VolumeDisplayUnit handler to handle two inputs.
    Maximum number of inputs will be two.
*/
class PairedVolumeDisplayUnitHandler : public GenericVolumeDisplayUnitHandler {
public:
    PairedVolumeDisplayUnitHandler();
    ~PairedVolumeDisplayUnitHandler();

    int getMaximumNumberOfInputs() const;
};

} // End namespace udg

#endif