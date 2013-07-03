#ifndef VOLUMEPIXELDATAREADERSELECTOR_H
#define VOLUMEPIXELDATAREADERSELECTOR_H

#include "volumepixeldatareaderfactory.h"

namespace udg {

class Volume;

/**
 * This class is used by VolumePixelDataReaderFactory to select the appropriate VolumePixelDataReader for a given volume.
 */
class VolumePixelDataReaderSelector {

public:
    virtual ~VolumePixelDataReaderSelector() {}

    /// Chooses and returns the reader implementation most suitable to the given volume.
    virtual VolumePixelDataReaderFactory::PixelDataReaderType selectVolumePixelDataReader(Volume *volume) const = 0;

};

} // namespace udg

#endif
