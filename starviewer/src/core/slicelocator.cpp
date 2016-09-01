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

void SliceLocator::setPlane(const OrthogonalPlane &plane)
{
    m_volumePlane = plane;
}

int SliceLocator::getNearestSlice(double point[3])
{
    if (!m_volume)
    {
        return -1;
    }
    
    double nearestSliceDistance = MathTools::DoubleMaximumValue;
    int nearestSlice = -1;
    
    for (int i = 0; i <= m_volume->getMaximumSlice(m_volumePlane); ++i)
    {
        ImagePlane *currentPlane = m_volume->getImagePlane(i, m_volumePlane);
        if (currentPlane)
        {
            double currentDistance = currentPlane->getDistanceToPoint(point);
            if (currentDistance < nearestSliceDistance)
            {
                nearestSliceDistance = currentDistance;
                nearestSlice = i;
            }

            delete currentPlane;
        }
    }

    if (isWithinProximityBounds(nearestSliceDistance))
    {
        return nearestSlice;
    }
    else
    {
        return -1;
    }
}

int SliceLocator::getNearestSlice(ImagePlane *imagePlane)
{
    if (!imagePlane)
    {
        return -1;
    }

    return getNearestSlice(imagePlane->getCenter().toArray().data());
}

bool SliceLocator::isWithinProximityBounds(double distanceToSlice)
{
    if (!m_volume)
    {
        return false;
    }

    int zIndex = m_volumePlane.getZIndex();

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
