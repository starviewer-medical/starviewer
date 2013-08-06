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

    /// Returns the nearest slice to the given point in the specified plane. The out parameter distance will have the distance from the point to the slice
    /// The nearest slice will be computed against the given volume in setVolume() method.
    /// If no slice is found, -1 will be returned
    int getNearestSlice(double point[3], OrthogonalPlane::OrthogonalPlaneType plane, double &distance);

private:
    /// The volume to compute slices from
    Volume *m_volume;
};

} // End namespace udg

#endif
