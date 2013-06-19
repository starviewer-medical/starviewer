#ifndef ITKGDCMBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H
#define ITKGDCMBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H

#include "volumepixeldatareaderselector.h"

namespace udg {

/**
 * This class is used by VolumePixelDataReaderFactory to select the appropriate VolumePixelDataReader for a given volume.
 * This selector chooses ITK-GDCM by default.
 */
class ItkGdcmByDefaultVolumePixelDataReaderSelector : public VolumePixelDataReaderSelector {

public:

    /// Chooses and returns the reader implementation most suitable to the given volume.
    virtual VolumePixelDataReaderFactory::PixelDataReaderType selectVolumePixelDataReader(Volume *volume) const;

};

} // namespace udg

#endif // ITKGDCMBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H
