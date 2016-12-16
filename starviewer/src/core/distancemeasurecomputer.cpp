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
    
    auto p1 = amendCoordinate(m_line->getFirstPoint(), dataSpacing, desiredSpacing);
    auto p2 = amendCoordinate(m_line->getSecondPoint(), dataSpacing, desiredSpacing);

    return MathTools::getDistance3D(p1, p2);
}

int DistanceMeasureComputer::getMeasureDimensions()
{
    return 1;
}

}
