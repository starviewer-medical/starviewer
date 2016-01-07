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

#ifndef UDGSLICELOCATOR_H
#define UDGSLICELOCATOR_H

#include "orthogonalplane.h"

namespace udg {

class Volume;
class ImagePlane;

/**
    Class that given a point locates an specific slice from a Volume
 */
class SliceLocator {
public:
    SliceLocator();
    ~SliceLocator();

    /// Sets the volume to compute slices from
    void setVolume(Volume *volume);

    /// Sets the plane where the computings will be taken. If no plane set, it will be the XY plane by default.
    void setPlane(const OrthogonalPlane &plane);
    
    /// Returns the nearest slice to the given point or ImagePlane.
    /// The nearest slice will be computed against the given volume and plane from setVolume() and setPlane() methods.
    /// If no slice is found to be considered near, -1 will be returned
    int getNearestSlice(double point[3]);
    int getNearestSlice(ImagePlane *imagePlane);

private:
    /// Returns true if the given slice distance could be considered to be within a certain proximity
    /// regarding the slice spacing values of the current volume, false otherwise
    bool isWithinProximityBounds(double distanceToSlice);

private:
    /// This factor keeps the proportion factor of slices to determine if an slice could be considered to be near to another slice or not
    static const double SliceProximityFactor;
    
    /// The volume to compute slices from
    Volume *m_volume;
    
    /// The plane upon computing will be taken
    OrthogonalPlane m_volumePlane;
};

} // End namespace udg

#endif
