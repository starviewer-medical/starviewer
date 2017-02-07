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

#ifndef UDGSLICEHANDLER_H
#define UDGSLICEHANDLER_H

#include "orthogonalplane.h"
#include "plane.h"

namespace udg {

class Image;
class Volume;

/**
 * @brief The SliceHandler class eases the navigation along a view plane normal with absolute positioning or slices. It supports phases and slab thickness.
 */
class SliceHandler {

public:

    SliceHandler();
    virtual ~SliceHandler();

    /// Sets the volume to work with and resets all fields to the default values for this volume
    /// (view plane with normal (0, 0, 1) and origin at volume origin, position 0, step distance to z-spacing, no slab thickness, and first phase).
    void setVolume(Volume *volume);

    /// Returns the reference view plane.
    const Plane& getReferenceViewPlane() const;
    /// Sets the reference view plane.
    void setReferenceViewPlane(Plane viewPlane);

    /// Returns the orthogonal plane corresponding to the reference view plane.
    const OrthogonalPlane& getOrthogonalViewPlane() const;
    /// Sets the reference view plane to the equivalent of the given orthogonal plane.
    void setOrthogonalViewPlane(const OrthogonalPlane &viewPlane);

    /// Returns the current position.
    double getPosition() const;
    /// Sets the current position bounded by minimum and maximum position.
    /// If snap to slice is on it will also adjust the position to match a slice according to the current step distance.
    void setPosition(double position);

    /// Returns the minimum allowed position taking into account volume size, the reference view plane and slab thickness.
    double getMinimumPosition() const;
    /// Returns the maximum allowed position taking into account volume size, the reference view plane and slab thickness.
    double getMaximumPosition() const;

    /// Returns the default step distance according to the current volume and reference view plane. The default step distance is the spacing in the normal
    /// direction if the reference view plane is orthogonal, and a combination of the spacing in each direction otherwise.
    double getDefaultStepDistance() const;

    /// Returns the current step distance.
    double getStepDistance() const;
    /// Sets the current step distance.
    void setStepDistance(double stepDistance);

    /// Returns the slice (according to the step distance) closest to the current position.
    int getSlice() const;
    /// Sets the position to match the given slice bounded by minimum and maximum slice (according to the step distance).
    /// If loop for slices is enabled in settings and the given slice is out of bounds, it will loop.
    void setSlice(int slice);

    /// Returns the minimum allowed slice taking into account volume size, the reference view plane, step distance and slab thickness.
    int getMinimumSlice() const;
    /// Returns the maximum allowed slice taking into account volume size, the reference view plane, step distance and slab thickness.
    int getMaximumSlice() const;
    
    /// Returns the number of slices taking into account volume size, the reference view plane and current step distance, but not slab thickness.
    int getNumberOfSlices() const;
    
    /// Returns the current phase.
    int getPhase() const;
    /// Sets the phase bounded by the number of phases. If loop for phases is enabled in settings and the given phase is out of bounds, it will loop.
    void setPhase(int phase);

    /// Returns the number of phases in the current volume.
    int getNumberOfPhases() const;
    
    /// Returns the current slab thickness.
    double getSlabThickness() const;
    /// Sets the slab thickness to the given value limited to the maximum slab thickness.
    /// Adjusts position so that the entire slab is within position bounds, snapping to a slice if needed.
    void setSlabThickness(double thickness);

    /// Returns the maximum allowed slab thickness.
    double getMaximumSlabThickness() const;

    /// Returns true if snap to slice is enabled and false otherwise.
    bool getSnapToSlice() const;
    /// Enables or disables snap to slice.
    void setSnapToSlice(bool snapToSlice);

    /// Returns the slice thickness of the currently displayed image. If the current position corresponds to an image in the volume, then DICOM Slice Thickness
    /// is returned. Otherwise it's computed from a combination of the spacing in X and Y and the slice thickness of the first image.
    double getSliceThickness() const;

    /// Returns the volume image corresponding to the current position, or null if there's no such image.
    Image* getImage() const;

protected:

    /// Returns true if loop is enabled for slices in settings and false otherwise.
    virtual bool isLoopEnabledForSlices() const;
    /// Returns true if loop is enabled for phases in settings and false otherwise.
    virtual bool isLoopEnabledForPhases() const;

private:

    /// Returns true if the given slab thickness value is valid and false otherwise.
    bool isValidSlabThickness(double thickness);

protected:

    /// The volume with the images and image data.
    Volume *m_volume;

    /// The view plane that defines the normal and position 0.
    Plane m_referenceViewPlane;
    /// Orthogonal plane corresponding to the reference view plane.
    OrthogonalPlane m_orthogonalViewPlane;

    /// Signed distance to the reference plane along its normal.
    double m_position;
    /// Minimum position computed from the volume and the reference view plane.
    double m_minPosition;
    /// Maximum position computed from the volume and the reference view plane.
    double m_maxPosition;

    /// The step distance when navigating by slices. Slice s corresponds to position s * step distance.
    double m_stepDistance;

    /// Current phase.
    int m_phase;
    /// Number of phases in the volume.
    int m_numberOfPhases;

    /// Slab thickness.
    double m_slabThickness;

    /// If true, setPosition and setSlabThickness will round the position to the nearest multiple of step distance.
    bool m_snapToSlice;

};

} // End namespace udg

#endif
