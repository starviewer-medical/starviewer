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
    
    /// Returns the nearest slice to the given point. The out parameter distance will have the distance from the point to the slice
    /// The nearest slice will be computed against the given volume and plane from setVolume() and setPlane() methods.
    /// If no slice is found, -1 will be returned
    int getNearestSlice(double point[3], double &distance);

private:
    /// The volume to compute slices from
    Volume *m_volume;
    
    /// The plane upon computing will be taken
    OrthogonalPlane::OrthogonalPlaneType m_volumePlane;
};

} // End namespace udg

#endif
