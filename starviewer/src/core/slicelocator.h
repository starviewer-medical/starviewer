#ifndef UDGSLICELOCATOR_H
#define UDGSLICELOCATOR_H

#include "orthogonalplane.h"

namespace udg {

class Volume;

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
    void setPlane(OrthogonalPlane::OrthogonalPlaneType plane);
    
    /// Returns the nearest slice to the given point.
    /// The nearest slice will be computed against the given volume and plane from setVolume() and setPlane() methods.
    /// If no slice is found to be considered near, -1 will be returned
    int getNearestSlice(double point[3]);

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
    OrthogonalPlane::OrthogonalPlaneType m_volumePlane;
};

} // End namespace udg

#endif
