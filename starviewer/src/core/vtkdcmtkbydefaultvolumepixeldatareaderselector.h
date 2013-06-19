#ifndef VTKDCMTKBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H
#define VTKDCMTKBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H

#include "volumepixeldatareaderselector.h"

namespace udg {

/**
 * This class is used by VolumePixelDataReaderFactory to select the appropriate VolumePixelDataReader for a given volume.
 * This selector chooses VTK-DCMTK by default.
 */
class VtkDcmtkByDefaultVolumePixelDataReaderSelector : public VolumePixelDataReaderSelector {

public:

    /// Chooses and returns the reader implementation most suitable to the given volume.
    virtual VolumePixelDataReaderFactory::PixelDataReaderType selectVolumePixelDataReader(Volume *volume) const;

};

} // namespace udg

#endif // VTKDCMTKBYDEFAULTVOLUMEPIXELDATAREADERSELECTOR_H
