#include "vtkdcmtkbydefaultvolumepixeldatareaderselector.h"

#include "image.h"
#include "volume.h"

namespace udg {

VolumePixelDataReaderFactory::PixelDataReaderType VtkDcmtkByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader(Volume *volume) const
{
    if (volume->getImage(0)->getSOPInstanceUID().contains("MHDImage"))
    {
        // MetaImages currently must be read with ITK-GDCM
        return VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
    else
    {
        // Read with VTK-DCMTK by default
        return VolumePixelDataReaderFactory::VTKDCMTKPixelDataReader;
    }
}

} // namespace udg
