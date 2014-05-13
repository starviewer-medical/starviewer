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

#include "itkgdcmbydefaultvolumepixeldatareaderselector.h"

#include "image.h"
#include "logging.h"
#include "series.h"
#include "volume.h"

namespace udg {

VolumePixelDataReaderFactory::PixelDataReaderType ItkGdcmByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader(Volume *volume) const
{
    int firstImageRows = 0;
    int firstImageColumns = 0;
    QList<Image*> imageSet = volume->getImages();
    if (!imageSet.empty())
    {
        firstImageRows = imageSet.first()->getRows();
        firstImageColumns = imageSet.first()->getColumns();
    }

    bool containsDifferentSizeImages = false;
    bool containsColorImages = false;
    bool avoidWrongPixelType = false;

    foreach (Image *image, imageSet)
    {
        // Check for images of different size
        // TODO This check could in theory be removed because images of different size are separated in different volumes, but we are keeping it just in case
        // there's a bug in the separation process
        if (firstImageRows != image->getRows() || firstImageColumns != image->getColumns())
        {
            DEBUG_LOG("Images of different size!");
            containsDifferentSizeImages = true;
        }

        // Check for color image
        // TODO Should also check for cases in which there are images of different size and some of them have color
        if (image->getPhotometricInterpretation().isColor())
        {
            // If photometric interpretation is not MONOCHROME1 nor MONOCHROME2, then it's a color image: PALETTE COLOR, RGB, YBR_FULL, YBR_FULL_422,
            // YBR_PARTIAL_422, YBR_PARTIAL_420, YBR_ICT or YBR_RCT
            containsColorImages = true;
            DEBUG_LOG("Photometric Interpretation: " + image->getPhotometricInterpretation().getAsQString());
        }
        else if (image->getBitsAllocated() == 16 && image->getBitsStored() == 16 && !image->getSOPInstanceUID().contains("MHDImage"))
        {
            // This check is performed to avoid cases like in ticket #1257
            // itk::Image has a fixed pixel type, currently signed short, but with 16 bits allocated and 16 bits stored the needed type might be unsigned short
            // in order for the image to be correctly displayed

            // Apply only for "non-volumetric" image modalities to narrow this problem and avoid the problem described in ticket #1313
            QStringList supportedModalities;
            supportedModalities << "CR" << "RF" << "DX" << "MG" << "OP" << "US" << "ES" << "NM" << "DT" << "PT" << "XA" << "XC";
            if (supportedModalities.contains(image->getParentSeries()->getModality()))
            {
                avoidWrongPixelType = true;
            }
        }
    }

    if (!containsDifferentSizeImages && containsColorImages)
    {
        // If there are color images and all of the same size, read with VTK-GDCM
        return VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }
    else if (avoidWrongPixelType)
    {
        // VTK-GDCM reader decides pixel type on run time, so it can choose the correct type
        return VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }
    else
    {
        // TODO By now, read with ITK-GDCM by default in all cases not treated above
        return VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
}

} // namespace udg
