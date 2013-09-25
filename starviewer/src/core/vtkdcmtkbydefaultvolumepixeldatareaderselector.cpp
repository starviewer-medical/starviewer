#include "vtkdcmtkbydefaultvolumepixeldatareaderselector.h"

#include "image.h"
#include "volume.h"

namespace udg {

namespace {

// Returns true if the volume contains at least one image with a JPEG2000 transfer syntax.
bool hasJPEG2000TransferSyntax(const Volume *volume)
{
    const QString JPEG2000LosslessOnlyTransferSyntax = "1.2.840.10008.1.2.4.90";
    const QString JPEG2000TransferSyntax = "1.2.840.10008.1.2.4.91";

    foreach (Image *image, volume->getImages())
    {
        if (image->getTransferSyntaxUID() == JPEG2000LosslessOnlyTransferSyntax || image->getTransferSyntaxUID() == JPEG2000TransferSyntax)
        {
            return true;
        }
    }

    return false;
}

}

VolumePixelDataReaderFactory::PixelDataReaderType VtkDcmtkByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader(Volume *volume) const
{
    if (volume->getImage(0)->getSOPInstanceUID().contains("MHDImage"))
    {
        // MetaImages currently must be read with ITK-GDCM
        return VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
    else if (hasJPEG2000TransferSyntax(volume))
    {
        // DCMTK doesn't support JPEG2000 for free
        return VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }
    else
    {
        // Read with VTK-DCMTK by default
        return VolumePixelDataReaderFactory::VTKDCMTKPixelDataReader;
    }
}

} // namespace udg
