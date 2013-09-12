#ifndef UDGPETCTVOLUMEDISPLAYUNITHANDLER_H
#define UDGPETCTVOLUMEDISPLAYUNITHANDLER_H

#include "pairedvolumedisplayunithandler.h"

namespace udg {

/**
    Specific VolumeDisplayUnit handler to handle two inputs, where the modalities of each one should be CT and PET.
    Maximum number of inputs will be two.
*/
class PETCTVolumeDisplayUnitHandler : public PairedVolumeDisplayUnitHandler {
public:
    PETCTVolumeDisplayUnitHandler();
    ~PETCTVolumeDisplayUnitHandler();

protected:
    /// Implementation of the corresponding virtual methods to configure the default transfer functions and the main input index
    void setupDefaultTransferFunctions();
    void updateMainDisplayUnitIndex();

private:
    /// Returns the display unit corresponding to the PET input
    VolumeDisplayUnit* getPETDisplayUnit() const;
};

} // End namespace udg

#endif
