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

#include "areameasurecomputer.h"

#include "drawerpolygon.h"
#include "pixelspacing2d.h"

namespace udg {

AreaMeasureComputer::AreaMeasureComputer(DrawerPolygon *polygon)
{
    m_polygon = polygon;
}

AreaMeasureComputer::~AreaMeasureComputer()
{
}

double AreaMeasureComputer::computeMeasure(Image *image, double dataSpacing[3])
{
    return computeMeasureExplicit(dataSpacing, getMeasureSpacing(image, dataSpacing));
}

double AreaMeasureComputer::computeMeasureExplicit(double dataSpacing[3], const PixelSpacing2D &desiredSpacing)
{
    if (!m_polygon)
    {
        return 0.0;
    }
    
    // First we guess on which plane is lying the m_polygon
    int xIndex;
    int yIndex;
    m_polygon->get2DPlaneIndices(xIndex, yIndex);
    if (xIndex == -1 || yIndex == -1)
    {
        // For safety
        xIndex = 0;
        yIndex = 1;
    }
    // Now we can compute the 2D area
    double area = 0.0;
    int j = 0;
    int numberOfPoints = m_polygon->getNumberOfPoints();
    for (int i = 0; i < numberOfPoints; i++)
    {
        j++;
        if (j == numberOfPoints)
        {
            j = 0;
        }

        auto p1 = amendCoordinate(m_polygon->getVertex(i), dataSpacing, desiredSpacing);
        auto p2 = amendCoordinate(m_polygon->getVertex(j), dataSpacing, desiredSpacing);

        area += (p1[xIndex] + p2[xIndex]) * (p1[yIndex] - p2[yIndex]);
    }
    
    return qAbs(area) * 0.5;
}

int AreaMeasureComputer::getMeasureDimensions()
{
    return 2;
}

}
