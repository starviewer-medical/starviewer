#ifndef UDGDISTANCEMEASURECOMPUTER_H
#define UDGDISTANCEMEASURECOMPUTER_H

#include "measurecomputer.h"

namespace udg {

class DrawerLine;

/**
 * Computes the distance from a DrawerLine
 */
class DistanceMeasureComputer : public MeasureComputer {
public:
    DistanceMeasureComputer(DrawerLine *line);
    ~DistanceMeasureComputer();

    double computeMeasure(Image *image, double dataSpacing[3]);

    double computeMeasureExplicit(double dataSpacing[3], const PixelSpacing2D &desiredSpacing);

    int getMeasureDimensions();

private:
    DrawerLine *m_line;
};

} // End namespace udg

#endif
