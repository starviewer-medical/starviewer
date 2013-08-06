#include "slicelocator.h"

#include "imageplane.h"
#include "mathtools.h"
#include "volume.h"

namespace udg {

SliceLocator::SliceLocator()
{
    m_volume = 0;
    m_volumePlane = OrthogonalPlane::XYPlane;
}

SliceLocator::~SliceLocator()
{
}

void SliceLocator::setVolume(Volume *volume)
{
    m_volume = volume;
}

void SliceLocator::setPlane(OrthogonalPlane::OrthogonalPlaneType plane)
{
    m_volumePlane = plane;
}

int SliceLocator::getNearestSlice(double point[3], double &distance)
{
    distance = MathTools::DoubleMaximumValue;
    
    if (!m_volume)
    {
        return -1;
    }
    
    double currentDistance;
    int nearestSlice = -1;
    ImagePlane *currentPlane = 0;
    int maximumSlice = m_volume->getMaximumSlice(m_volumePlane);

    for (int i = 0; i <= maximumSlice; i++)
    {
        currentPlane = m_volume->getImagePlane(i, m_volumePlane);
        if (currentPlane)
        {
            currentDistance = currentPlane->getDistanceToPoint(point);
            if (currentDistance < distance)
            {
                distance = currentDistance;
                nearestSlice = i;
            }

            delete currentPlane;
        }
    }

    return nearestSlice;
}

} // End namespace udg
