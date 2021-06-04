#ifndef UDG_NMCTVOLUMEDISPLAYUNITHANDLER_H
#define UDG_NMCTVOLUMEDISPLAYUNITHANDLER_H

#include "pairedvolumedisplayunithandler.h"

namespace udg {

/**
    Specific VolumeDisplayUnit handler to handle two inputs, where the modalities of each one should be CT and NM.
    Maximum number of inputs will be two.
*/
class NMCTVolumeDisplayUnitHandler : public PairedVolumeDisplayUnitHandler
{

    Q_OBJECT

public:
    NMCTVolumeDisplayUnitHandler(QObject *parent = nullptr);
    virtual ~NMCTVolumeDisplayUnitHandler();

protected:
    /// Implementation of the corresponding virtual methods to configure the default transfer functions and the main input index
    virtual void setupDefaultTransferFunctions();
    virtual void updateMainDisplayUnitIndex();

private:
    /// Returns the display unit corresponding to the NM input
    VolumeDisplayUnit* getNMDisplayUnit() const;
};

} // namespace udg

#endif // UDG_NMCTVOLUMEDISPLAYUNITHANDLER_H
