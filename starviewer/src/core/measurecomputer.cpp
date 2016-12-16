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

#include "measurecomputer.h"

#include "image.h"
#include "pixelspacing2d.h"

namespace udg {

MeasureComputer::MeasureComputer()
{
}

MeasureComputer::~MeasureComputer()
{
}

Vector3 MeasureComputer::amendCoordinate(const Vector3 &coordinate, double coordinateSpacing[3], const PixelSpacing2D &amenderSpacing)
{
    if (!coordinateSpacing)
    {
        return coordinate;
    }
    
    Vector3 amendedCoordinate;
    double multiplierFactor[2];
    
    if (!amenderSpacing.isValid())
    {
        multiplierFactor[0] = multiplierFactor[1] = 1.0;
        
        amendedCoordinate.z = coordinate.z / coordinateSpacing[2];
    }
    else
    {
        multiplierFactor[0] = amenderSpacing.x();
        multiplierFactor[1] = amenderSpacing.y(); 

        amendedCoordinate.z = coordinate.z;
    }
    
    for (int i = 0; i < 2; ++i)
    {
        amendedCoordinate[i] = coordinate[i] / coordinateSpacing[i] * multiplierFactor[i];
    }
    
    return amendedCoordinate;
}

PixelSpacing2D MeasureComputer::getMeasureSpacing(Image *image, double dataSpacing[3])
{
    PixelSpacing2D measureSpacing;
    if (!image)
    {
        measureSpacing.setX(dataSpacing[0]);
        measureSpacing.setX(dataSpacing[1]);
    }
    else
    {
        measureSpacing = image->getPreferredPixelSpacing();
    }

    return measureSpacing;
}

}
