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

#ifndef VOLUMEDISPLAYUNIT_H
#define VOLUMEDISPLAYUNIT_H

#include <QObject>

#include "voilut.h"

#include <vtkSystemIncludes.h>

class vtkCamera;
class vtkImageData;
class vtkImageSlice;
class vtkImageStack;
class vtkPropPicker;

namespace udg {

class Image;
class ImagePipeline;
class OrthogonalPlane;
class SliceHandler;
class SliceOrientedVolumePixelData;
class Volume;
class VoiLutPresetsToolData;
class VolumePixelData;
class VtkImageResliceMapper2;

/**
    This class groups together a Volume and the associated objects that a Q2DViewer needs to display a volume.
    The ImagePipeline, the vtkImageSlice and the SliceHandler are created internally in the constructor and destroyed in the destructor, so only the Volume has
    to be supplied externally.
 */
class VolumeDisplayUnit : public QObject {

    Q_OBJECT

public:
    /// Supported projection modes for thick slab.
    enum SlabProjectionMode { Max = VTK_IMAGE_SLAB_MAX, Min = VTK_IMAGE_SLAB_MIN, Mean = VTK_IMAGE_SLAB_MEAN, Sum = VTK_IMAGE_SLAB_SUM };

    VolumeDisplayUnit(QObject *parent = nullptr);
    virtual ~VolumeDisplayUnit();

    /// Returns the volume.
    Volume* getVolume() const;
    /// Sets a new volume and resets display properties (pipeline and slice handler).
    void setVolume(Volume *volume);

    /// Returns the VOI LUT data.
    VoiLutPresetsToolData* getVoiLutData() const;
    /// Sets a new VOI LUT data.
    void setVoiLutData(VoiLutPresetsToolData *voiLutData);

    /// Returns the image pipeline.
    ImagePipeline* getImagePipeline() const;

    /// Returns the main vtkImageSlice.
    vtkImageSlice* getImageSlice() const;

    /// Returns the image stack.
    vtkImageStack* getImageStack() const;

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
    SliceOrientedVolumePixelData getCurrentPixelData();

    /// Restores the standard rendering quality (resample to screen pixels on) of this volume display unit.
    void restoreRenderingQuality();
    
    /// Returns current displayed image.
    /// If some orthogonal reconstruction different from original acquisition is applied, returns null
    Image* getCurrentDisplayedImage() const;

    /// Updates the displayed image in the image slice.
    virtual void updateImageSlice(vtkCamera *camera);

    /// Updates the current image default presets values. It only applies to original acquisition plane.
    void updateCurrentImageDefaultPresets();

    /// Sets the given VOI LUT to the pipeline.
    void setVoiLut(const VoiLut &voiLut);
    /// Sets the given VOI LUT to the VOI LUT data and to the pipeline.
    void setCurrentVoiLutPreset(const VoiLut &voiLut);

    /// Sets the transfer function.
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Returns the transfer function.
    const TransferFunction& getTransferFunction() const;
    /// Clears the transfer function.
    void clearTransferFunction();

    /// Returns the current slice index.
    int getSlice() const;
    /// Sets the current slice index.
    void setSlice(int slice);

    /// Returns the minimum slice index.
    int getMinimumSlice() const;
    /// Returns the maximum slice index.
    int getMaximumSlice() const;

    /// Returns the total number of slices in the spatial dimension for the current view plane.
    int getNumberOfSlices() const;

    /// Returns the current phase index.
    int getPhase() const;
    /// Sets the current phase index.
    void setPhase(int phase);

    /// Returns the number of phases in the temporal dimension.
    int getNumberOfPhases() const;

    /// Returns the slab thickness in mm.
    double getSlabThickness() const;
    /// Sets the slab thickness in mm.
    void setSlabThickness(double thickness);

    /// Returns the maximum slab thickness that can be set.
    double getMaximumSlabThickness() const;

    /// Returns true i slab thickness is greater than 1
    bool isThickSlabActive() const;

    /// Returns slice thickness of the currently displayed image.
    /// On the acquisition plane, this depends on DICOM's slice thickness and slab thickness (if DICOM's slice thickness is not defined, the method returns 0).
    /// On the other planes, this depends on the spacing and the slab thickness.
    double getSliceThickness() const;

    /// Sets the slab projection mode for the thick slab.
    void setSlabProjectionMode(SlabProjectionMode mode);

    /// Sets the display shutter image data.
    void setShutterData(vtkImageData *shutterData);

    /// Returns true if this volume display unit is visible and false otherwise.
    bool isVisible() const;
    /// Sets the visibility of this volume display unit to the given value.
    void setVisible(bool visible);

    /// Sets the opacity of this volume display unit.
    void setOpacity(double opacity);

signals:
    /// Emitted when the composition of the image stack has changed, i.e. when the shutter is enabled or disabled.
    void imageStackChanged();

protected:
    /// The volume.
    Volume *m_volume;

    /// The image actor where the slices are rendered.
    vtkImageSlice *m_imageSlice;

    /// The mapper for the slice.
    VtkImageResliceMapper2 *m_mapper;

    /// The slice handler that controls slices, phases and slabs.
    SliceHandler *m_sliceHandler;

private:
    /// Called when setting a new volume to reset the thick slab filter.
    void resetThickSlab();

    void setupPicker();

    /// Applies the current VOI LUT taking into account the properties of the image, the VOI LUT and the transfer function.
    void applyVoiLut();
    /// Modifies the current transfer function according to the current window level and applies it.
    void applyTransferFunction();

private:
    /// The image pipeline that processes the volume.
    ImagePipeline *m_imagePipeline;

    /// Used to render the shutter.
    vtkImageSlice *m_shutterImageSlice;
    /// Used to compose the shutter on top of the main image.
    vtkImageStack *m_imageStack;

    /// Point picker to probe pixels from the image to display
    vtkPropPicker *m_imagePointPicker;

    /// VOI LUT data.
    VoiLutPresetsToolData *m_voiLutData;

    /// The current VOI LUT.
    VoiLut m_voiLut;

    /// The current transfer function.
    TransferFunction m_transferFunction;

    /// Holds the current volume pixel data to return in case of thick slab or phases.
    VolumePixelData *m_auxiliarCurrentVolumePixelData;

};

}

#endif
