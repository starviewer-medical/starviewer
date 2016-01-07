/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "vtkdcmtkbydefaultvolumepixeldatareaderselector.h"

#include "dicomtagreader.h"
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

// Returns true if the images in the volume have a segmented palette attribute.
// For speed reasons, it just checks for the presence of the segmented red palette in the first image.
bool hasSegmentedPalette(const Volume *volume)
{
    // We only need to check for the tag if the photometric interpretion is palette color
    if (volume->getImage(0)->getPhotometricInterpretation() == PhotometricInterpretation::Palette_Color)
    {
        DICOMTagReader tagReader(volume->getImage(0)->getPath());
        return tagReader.tagExists(DICOMSegmentedRedPaletteColorLookupTableData);
    }
    else
    {
        return false;
    }
}

}

VolumePixelDataReaderFactory::PixelDataReaderType VtkDcmtkByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader(Volume *volume) const
{
    if (volume->getImage(0)->getSOPInstanceUID().contains("MHDImage"))
    {
        // MetaImages currently must be read with ITK-GDCM
        return VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
    else if (hasJPEG2000TransferSyntax(volume) || hasSegmentedPalette(volume))
    {
        // DCMTK doesn't support JPEG2000 for free, and doesn't support segmented palettes either
        return VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }
    else
    {
        // Read with VTK-DCMTK by default
        return VolumePixelDataReaderFactory::VTKDCMTKPixelDataReader;
    }
}

} // namespace udg
