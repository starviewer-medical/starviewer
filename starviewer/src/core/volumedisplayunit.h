#ifndef VOLUMEDISPLAYUNIT_H
#define VOLUMEDISPLAYUNIT_H

#include "accumulator.h"
#include "windowlevel.h"

class vtkImageActor;
class vtkImageData;
class vtkPropPicker;

namespace udg {

class Image;
class ImagePipeline;
class OrthogonalPlane;
class SliceHandler;
class TransferFunction;
class Volume;
class WindowLevelPresetsToolData;
class VolumePixelData;

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

    /// Returns the configured point picker for this unit.
    vtkPropPicker* getImagePointPicker();
    
    /// Returns the view plane.
    const OrthogonalPlane& getViewPlane() const;
    /// Sets a new view plane, resetting some properties of the slice handler.
    void setViewPlane(const OrthogonalPlane &viewPlane);

    /// Gets the current spacing between slices according to the current view on the set volume
    double getCurrentSpacingBetweenSlices() const;
    
    /// Returns the depth (z coordinate value) of the displayed image
    double getCurrentDisplayedImageDepth() const;
    
    /// Gets the current pixel data according to the current state.
    VolumePixelData* getCurrentPixelData();
    
    /// Updates the display extent of the image actor.
    void updateDisplayExtent();

    /// Updates the current image default presets values. It only applies to original acquisition plane.
    void updateCurrentImageDefaultPresets();

    /// Updates the current window level
    void updateWindowLevel(const WindowLevel &windowLevel);

    /// Returns the current window level on the given array.
    void getWindowLevel(double windowLevel[2]) const;
    /// Sets the window level.
    void setWindowLevel(double window, double level);

    /// Sets the transfer function.
    void setTransferFunction(const TransferFunction &transferFunction);

    /// Returns the current slice index.
    int getSlice() const;
    /// Sets the current slice index.
    void setSlice(int slice);

    /// Returns the minimum slice index.
    int getMinimumSlice() const;
    /// Returns the maximum slice index that could be set, so it takes into account the current slice thickness.
    int getMaximumSlice() const;

    /// Returns the total number of slices in the spatial dimension for the current view plane.
    int getNumberOfSlices() const;

    /// Returns the current phase index.
    int getPhase() const;
    /// Sets the current phase index.
    void setPhase(int phase);

    /// Returns the number of phases in the temporal dimension.
    int getNumberOfPhases() const;

    /// Returns the number of slices that form a slab.
    int getSlabThickness() const;
    /// Sets the number of slices that form a slab.
    void setSlabThickness(int thickness);

    /// Returns true i slab thickness is greater than 1
    bool isThickSlabActive() const;

    /// Returns slice thickness of the currently displayed image.
    /// On the acquisition plane, this depends on DICOM's slice thickness and slab thickness (if DICOM's slice thickness is not defined, the method returns 0).
    /// On the other planes, this depends on the spacing and the slab thickness.
    double getSliceThickness() const;

    /// Sets the slab projection mode for the thick slab.
    void setSlabProjectionMode(AccumulatorFactory::AccumulatorType accumulatorType);

    /// Sets the display shutter image data.
    void setShutterData(vtkImageData *shutterData);

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
    
    /// Holds the current thickslab pixel data
    VolumePixelData *m_currentThickSlabPixelData;
};

}

#endif
