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

        double *p1 = amendCoordinate((double*)m_polygon->getVertix(i), dataSpacing, desiredSpacing);
        double *p2 = amendCoordinate((double*)m_polygon->getVertix(j), dataSpacing, desiredSpacing);

        area += (p1[xIndex] + p2[xIndex]) * (p1[yIndex] - p2[yIndex]);
    }
    
    return std::abs(area) * 0.5;
}

int AreaMeasureComputer::getMeasureDimensions()
{
    return 2;
}

}
