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
    enum PixelDataReaderType { ITKDCMTKPixelDataReader, ITKGDCMPixelDataReader, VTKDCMTKPixelDataReader, VTKGDCMPixelDataReader };

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
