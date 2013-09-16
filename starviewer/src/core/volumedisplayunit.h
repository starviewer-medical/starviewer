#ifndef VOLUMEDISPLAYUNIT_H
#define VOLUMEDISPLAYUNIT_H

#include "windowlevel.h"

class vtkImageActor;
class vtkPropPicker;

namespace udg {

class Image;
class ImagePipeline;
class OrthogonalPlane;
class SliceHandler;
class Volume;
class WindowLevelPresetsToolData;

/**
    This class groups together a Volume and the associated objects that a Q2DViewer needs to display a volume.
    The ImagePipeline, the vtkImageActor and the SliceHandler are created internally in the constructor and destroyed in the destructor, so only the Volume has
    to be supplied externally.
 */
class VolumeDisplayUnit {

public:
    VolumeDisplayUnit();
    ~VolumeDisplayUnit();

    /// Returns the volume.
    Volume* getVolume() const;
    /// Sets a new volume and resets display properties (pipeline and slice handler).
    void setVolume(Volume *volume);

    /// Returns the window level data
    WindowLevelPresetsToolData* getWindowLevelData();
    /// Sets a new window level data
    void setWindowLevelData(WindowLevelPresetsToolData *windowLevelData);

    /// Returns the image pipeline.
    ImagePipeline* getImagePipeline() const;

    /// Returns the image actor.
    vtkImageActor* getImageActor() const;

    /// Returns the slice handler.
    SliceHandler* getSliceHandler() const;

    /// Returns the configured point picker for this unit.
    vtkPropPicker* getImagePointPicker();
    
    /// Returns the view plane.
    const OrthogonalPlane& getViewPlane() const;
    /// Sets a new view plane, resetting some properties of the slice handler.
    void setViewPlane(const OrthogonalPlane &viewPlane);

    /// Updates the display extent of the image actor.
    void updateDisplayExtent();

    /// Updates the current image default presets values. It only applies to original acquisition plane.
    void updateCurrentImageDefaultPresets();

    /// Updates the current window level
    void updateWindowLevel(const WindowLevel &windowLevel);

private:
    /// Called when setting a new volume to reset the thick slab filter.
    void resetThickSlab();

    void setupPicker();

private:
    /// The volume.
    Volume *m_volume;

    /// The image pipeline that processes the volume.
    ImagePipeline *m_imagePipeline;

    /// The image actor where the slices are rendered.
    vtkImageActor *m_imageActor;

    /// The slice handler that controls slices, phases and slabs.
    SliceHandler *m_sliceHandler;

    /// Point picker to probe pixels from the image to display
    vtkPropPicker *m_imagePointPicker;

    /// Window and level data
    WindowLevelPresetsToolData *m_windowLevelData;
};

}

#endif
