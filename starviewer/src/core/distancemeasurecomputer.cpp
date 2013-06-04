#include "distancemeasurecomputer.h"

#include "drawerline.h"
#include "pixelspacing2d.h"
#include "mathtools.h"

namespace udg {

DistanceMeasureComputer::DistanceMeasureComputer(DrawerLine *line)
{
    m_line = line;
}

DistanceMeasureComputer::~DistanceMeasureComputer()
{
}

double DistanceMeasureComputer::computeMeasure(Image *image, double dataSpacing[3])
{
    return computeMeasureExplicit(dataSpacing, getMeasureSpacing(image, dataSpacing));
}

double DistanceMeasureComputer::computeMeasureExplicit(double dataSpacing[3], const PixelSpacing2D &desiredSpacing)
{
    if (!m_line)
    {
        return 0.0;
    }
    
    double *p1 = amendCoordinate(m_line->getFirstPoint(), dataSpacing, desiredSpacing);
    double *p2 = amendCoordinate(m_line->getSecondPoint(), dataSpacing, desiredSpacing);

    return MathTools::getDistance3D(p1, p2);
}

int DistanceMeasureComputer::getMeasureDimensions()
{
    return 1;
}

}
