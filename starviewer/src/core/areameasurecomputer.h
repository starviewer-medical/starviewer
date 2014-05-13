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
