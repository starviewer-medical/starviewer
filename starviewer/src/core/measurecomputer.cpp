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

double* MeasureComputer::amendCoordinate(double coordinate[3], double coordinateSpacing[3], const PixelSpacing2D &amenderSpacing)
{
    if (!coordinate)
    {
        return 0;
    }

    if (!coordinateSpacing)
    {
        double *sameCoordinate = new double[3];
        sameCoordinate[0] = coordinate[0];
        sameCoordinate[1] = coordinate[1];
        sameCoordinate[2] = coordinate[2];

        return sameCoordinate;
    }
    
    double *amendedCoordinate = new double[3];

    double multiplierFactor[2];
    
    if (!amenderSpacing.isValid())
    {
        multiplierFactor[0] = multiplierFactor[1] = 1.0;
        
        amendedCoordinate[2] = coordinate[2] / coordinateSpacing[2];
    }
    else
    {
        multiplierFactor[0] = amenderSpacing.x();
        multiplierFactor[1] = amenderSpacing.y(); 

        amendedCoordinate[2] = coordinate[2];
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
