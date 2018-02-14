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

#include "volumepixeldatareaderfactory.h"

#include "computezspacingpostprocessor.h"
#include "dicomtagreader.h"
#include "pixelspacingamenderpostprocessor.h"
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

void VolumePixelDataReaderFactory::setVolume(Volume *volume)
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

QQueue< QSharedPointer<Postprocessor> > VolumePixelDataReaderFactory::getPostprocessors() const
{
    QQueue< QSharedPointer<Postprocessor> > postprocessors;

    switch (m_chosenReaderType)
    {
        case ITKDCMTKPixelDataReader:
        case VTKDCMTKPixelDataReader:
        case VTKGDCMPixelDataReader:
            postprocessors.enqueue(QSharedPointer<Postprocessor>(new ComputeZSpacingPostprocessor()));
            postprocessors.enqueue(QSharedPointer<Postprocessor>(new PixelSpacingAmenderPostProcessor()));
            break;

        default:
            break;
    }

    return postprocessors;
}

VolumePixelDataReaderFactory::PixelDataReaderType VolumePixelDataReaderFactory::getSuitableReader(Volume *volume) const
{
    QScopedPointer<SettingsInterface> settings(this->getSettings());
    PixelDataReaderType readerType;

    // Start by checking if reader type is forced by settings
    // This is intended as a kind of backdoor to be able to provide a workaround in case there is a bug with the usually chosen reader
    if (!mustForceReaderLibraryBackdoor(volume, readerType))
    {
        // If the reader type is not forced by settings, decide
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

    return readerType;
}

bool VolumePixelDataReaderFactory::mustForceReaderLibraryBackdoor(Volume *volume, PixelDataReaderType &forcedReaderLibrary) const
{
    Q_ASSERT(volume);

    bool forceLibrary = false;
    QScopedPointer<SettingsInterface> settings(this->getSettings());

    // First check setting to force read everything with the same implementation
    QString forceReadingWithSpecfiedLibrary = settings->getValue(CoreSettings::ForcedImageReaderLibrary).toString().trimmed();
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
        QString modality = volume->getModality();

        // Check for modalities to force read with ITK-GDCM
        QStringList forceITKForModalities = settings->getValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
        if (forceITKForModalities.contains(modality))
        {
            INFO_LOG("Force read current volume with ITK-GDCM because its modality is " + modality);
            forcedReaderLibrary = ITKGDCMPixelDataReader;
            forceLibrary = true;
        }

        // If not forced read with ITK-GDCM, then check for VTK-GDCM
        if (!forceLibrary)
        {
            QStringList forceVTKForModalities = settings->getValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
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

SettingsInterface* VolumePixelDataReaderFactory::getSettings() const
{
    return new Settings();
}

} // namespace udg
