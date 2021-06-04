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

#ifndef VOLUMEPIXELDATAREADERFACTORY_H
#define VOLUMEPIXELDATAREADERFACTORY_H

#include <QSharedPointer>
#include <QQueue>

namespace udg {

class Postprocessor;
class SettingsInterface;
class Volume;
class VolumePixelDataReader;

/**
 * Factory that instantiates a specific VolumePixelDataReader implementation depending on a given Volume.
 */
class VolumePixelDataReaderFactory {

public:
    /// The different choosable implementations of VolumePixelDataReader.
    enum PixelDataReaderType { ITKDCMTKPixelDataReader, ITKGDCMPixelDataReader, VTKDCMTKPixelDataReader, VTKGDCMPixelDataReader, VTKPixelDataReader };

    /// Configures the factory according to the given volume, deciding the appropriate reader implementation. Call this before using getter methods.
    void setVolume(Volume *volume);

    /// Returns a new instance of the chosen reader implementation.
    VolumePixelDataReader* getReader() const;

    /// Returns the queue of postprocessors corresponding to the chosen reader implementation.
    QQueue< QSharedPointer<Postprocessor> > getPostprocessors() const;

private:
    /// Chooses and returns the reader implementation most suitable to the given volume.
    PixelDataReaderType getSuitableReader(Volume *volume) const;

    /// Returns whether there is a setting to force reading with a specific implementation. If true, that implementation is returned in the second parameter.
    /// The following settings are checked to determine forced reader type, in the following order: ForcedImageReaderLibrary,
    /// ForceITKImageReaderForSpecifiedModalities and ForceVTKImageReaderForSpecifiedModalities.
    /// Warning: use of these settings should be restricted to special cases such as to have a temporal workaround in case some reader is failing, or for
    /// debugging purposes and the like.
    bool mustForceReaderLibraryBackdoor(Volume *volume, PixelDataReaderType &forcedReaderLibrary) const;

    /// Creates and returns an object that implements SettingsInterface.
    virtual SettingsInterface* getSettings() const;

private:
    /// Chosen VolumePixelDataReader implementation.
    PixelDataReaderType m_chosenReaderType;

};

} // namespace udg

#endif
