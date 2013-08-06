#include "slicelocator.h"

#include "imageplane.h"
#include "mathtools.h"
#include "volume.h"

namespace udg {

/// With this value we consider an slice could be considered to be near if it's not greater than 1.5 slices far
const double SliceLocator::SliceProximityFactor = 1.5;

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

int SliceLocator::getNearestSlice(double point[3])
{
    if (!m_volume)
    {
        return -1;
    }
    
    double distance = MathTools::DoubleMaximumValue;
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

    if (isWithinProximityBounds(distance))
    {
        return nearestSlice;
    }
    else
    {
        return -1;
    }
}

bool SliceLocator::isWithinProximityBounds(double distanceToSlice)
{
    if (!m_volume)
    {
        return false;
    }

    int zIndex = OrthogonalPlane::getZIndexForView(m_volumePlane);

    double spacingBetweenSlices = m_volume->getSpacing()[zIndex];

    if (distanceToSlice < (spacingBetweenSlices * SliceProximityFactor))
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // End namespace udg
