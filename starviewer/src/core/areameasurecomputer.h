#ifndef UDGAREAMEASURECOMPUTER_H
#define UDGAREAMEASURECOMPUTER_H

#include "measurecomputer.h"

namespace udg {

class DrawerPolygon;

/**
 * Computes the area from a DrawerPolygon
 */
class AreaMeasureComputer : public MeasureComputer {
public:
    AreaMeasureComputer(DrawerPolygon *polygon);
    ~AreaMeasureComputer();

    double computeMeasure(Image *image, double dataSpacing[3]);

    double computeMeasureExplicit(double dataSpacing[3], const PixelSpacing2D &desiredSpacing);
    
    int getMeasureDimensions();

private:
    DrawerPolygon *m_polygon;
};

} // End namespace udg

#endif
