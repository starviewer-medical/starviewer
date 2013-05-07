#include "volumepixeldatareaderfactory.h"

#include "computezspacingpostprocessor.h"
#include "coresettings.h"
#include "image.h"
#include "logging.h"
#include "series.h"
#include "volume.h"
#include "volumepixeldatareaderitkdcmtk.h"
#include "volumepixeldatareaderitkgdcm.h"
#include "volumepixeldatareadervtkdcmtk.h"
#include "volumepixeldatareadervtkgdcm.h"

#include <QList>

namespace udg {

VolumePixelDataReaderFactory::VolumePixelDataReaderFactory(Volume *volume)
{
    m_chosenReaderType = getSuitableReader(volume);
}

VolumePixelDataReader* VolumePixelDataReaderFactory::getReader() const
{
    VolumePixelDataReader *reader = 0;

    switch (m_chosenReaderType)
    {
        case ITKDCMTKPixelDataReader:
            reader = new VolumePixelDataReaderITKDCMTK();
            DEBUG_LOG("Volume pixel data will be read using ITK-DCMTK");
            break;

        case ITKGDCMPixelDataReader:
            reader = new VolumePixelDataReaderITKGDCM();
            DEBUG_LOG("Volume pixel data will be read using ITK-GDCM");
            break;

        case VTKDCMTKPixelDataReader:
            reader = new VolumePixelDataReaderVTKDCMTK();
            DEBUG_LOG("Volume pixel data will be read using VTK-DCMTK");
            break;

        case VTKGDCMPixelDataReader:
            reader = new VolumePixelDataReaderVTKGDCM();
            DEBUG_LOG("Volume pixel data will be read using VTK-GDCM");
            break;
    }

    return reader;
}

QQueue<QSharedPointer<Postprocessor>> VolumePixelDataReaderFactory::getPostprocessors() const
{
    QQueue<QSharedPointer<Postprocessor>> postprocessors;

    switch (m_chosenReaderType)
    {
        case ITKDCMTKPixelDataReader:
        case VTKDCMTKPixelDataReader:
        case VTKGDCMPixelDataReader:
            postprocessors.enqueue(QSharedPointer<Postprocessor>(new ComputeZSpacingPostprocessor()));
            break;

        default:
            break;
    }

    return postprocessors;
}

VolumePixelDataReaderFactory::PixelDataReaderType VolumePixelDataReaderFactory::getSuitableReader(Volume *volume) const
{
    // Start by checking if reader type is forced by settings
    // This is intended as a kind of backdoor to be able to provide a workaround in case there is a bug with the usually chosen reader
    PixelDataReaderType forcedReaderLibrary;
    if (mustForceReaderLibraryBackdoor(volume, forcedReaderLibrary))
    {
        return forcedReaderLibrary;
    }

    QList<Image*> imageSet = volume->getImages();
    bool isMetaImage = false;
    bool containsColorImages = false;
    bool avoidWrongPixelType = false;

    foreach (Image *image, imageSet)
    {
        if (image->getSOPInstanceUID().contains("MHDImage"))
        {
            isMetaImage = true;
        }

        // Check for color image
        QString photometricInterpretation = image->getPhotometricInterpretation();
        if (!photometricInterpretation.contains("MONOCHROME"))
        {
            // If photometric interpretation is not MONOCHROME1 nor MONOCHROME2, then it's a color image: PALETTE COLOR, RGB, YBR_FULL, YBR_FULL_422,
            // YBR_PARTIAL_422, YBR_PARTIAL_420, YBR_ICT or YBR_RCT
            containsColorImages = true;
            DEBUG_LOG("Photometric Interpretation: " + photometricInterpretation);
        }

        // Check if maximum theoretical pixel value after rescale is representable by VoxelType
        int bits = image->getPixelRepresentation() == 0 ? image->getBitsStored() : image->getBitsStored() - 1;
        double maximumTheoreticalValue = image->getRescaleSlope() * (std::pow(2.0, bits) - 1.0) + image->getRescaleIntercept();
        if (maximumTheoreticalValue > std::numeric_limits<VolumePixelData::VoxelType>::max())
        {
            avoidWrongPixelType = true;
        }
    }

    if (isMetaImage)
    {
        // MetaImages currently must be read with ITK-GDCM
        return ITKGDCMPixelDataReader;
    }
    else if (containsColorImages || avoidWrongPixelType)
    {
        // If there are color images, read with VTK-GDCM
        // VTK-GDCM reader decides pixel type on run time, so it can choose the correct type
        return VTKGDCMPixelDataReader;
    }
    else if (volume->isMultiframe())
    {
        // If the volume is multiframe, read with ITK-DCMTK to avoid memory peaks that happen with GDCM
        return ITKDCMTKPixelDataReader;
    }
    else
    {
        // Read with VTK-GDCM by default in all cases not treated above
        return VTKGDCMPixelDataReader;
    }
}

bool VolumePixelDataReaderFactory::mustForceReaderLibraryBackdoor(Volume *volume, PixelDataReaderType &forcedReaderLibrary) const
{
    Q_ASSERT(volume);

    bool forceLibrary = false;
    Settings settings;

    // First check setting to force read everything with the same implementation
    QString forceReadingWithSpecfiedLibrary = settings.getValue(CoreSettings::ForcedImageReaderLibrary).toString().trimmed();
    if (forceReadingWithSpecfiedLibrary == "vtk")
    {
        INFO_LOG("Force read everything with VTK-GDCM");
        forcedReaderLibrary = VTKGDCMPixelDataReader;
        forceLibrary = true;
    }
    else if (forceReadingWithSpecfiedLibrary == "itk")
    {
        INFO_LOG("Force read everything with ITK-GDCM");
        forcedReaderLibrary = ITKGDCMPixelDataReader;
        forceLibrary = true;
    }
    else if (forceReadingWithSpecfiedLibrary == "itkdcmtk")
    {
        INFO_LOG("Force read everything with ITK-DCMTK");
        forcedReaderLibrary = ITKDCMTKPixelDataReader;
        forceLibrary = true;
    }
    else if (forceReadingWithSpecfiedLibrary == "vtkdcmtk")
    {
        INFO_LOG("Force read everything with VTK-DCMTK");
        forcedReaderLibrary = VTKDCMTKPixelDataReader;
        forceLibrary = true;
    }

    // If previous setting is not set, check the next, to force read all images of a specific modality with a specific implementation
    if (!forceLibrary)
    {
        // Get modality of current volume
        QString modality;
        Image *firstImage = volume->getImage(0);
        if (firstImage)
        {
            modality = firstImage->getParentSeries()->getModality();
        }

        // Check for modalities to force read with ITK-GDCM
        QStringList forceITKForModalities = settings.getValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
        if (forceITKForModalities.contains(modality))
        {
            INFO_LOG("Force read current volume with ITK-GDCM because its modality is " + modality);
            forcedReaderLibrary = ITKGDCMPixelDataReader;
            forceLibrary = true;
        }

        // If not forced read with ITK-GDCM, then check for VTK-GDCM
        if (!forceLibrary)
        {
            QStringList forceVTKForModalities = settings.getValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
            if (forceVTKForModalities.contains(modality))
            {
                INFO_LOG("Force read current volume with VTK-GDCM because its modality is " + modality);
                forcedReaderLibrary = VTKGDCMPixelDataReader;
                forceLibrary = true;
            }
        }
    }

    return forceLibrary;
}

} // namespace udg
