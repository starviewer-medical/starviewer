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

#ifndef UDGMEASURECOMPUTER_H
#define UDGMEASURECOMPUTER_H

#include "vector3.h"

namespace udg {

class Image;
class PixelSpacing2D;

/**
 * Abstract interface to compute specific measures.
 * Inherited classes should implement computeMeasure*() and getMeasureDimensions() methods and provide the object to take measures on.
 * For example, if a perimeter MeasureComputer is implemented, the object representing the shape to compute the perimeter of, should be specified.
 */
class MeasureComputer {
public:
    MeasureComputer();
    virtual ~MeasureComputer();

    /// Computes the measure where points have been taken in a volume with dataSpacing, using the preferred pixel spacing information from the given image
    virtual double computeMeasure(Image *image, double dataSpacing[3]) = 0;

    /// Computes the measure where points have been taken in a volume with dataSpacing using desiredSpacing as the reference spacing for the measure
    virtual double computeMeasureExplicit(double dataSpacing[3], const PixelSpacing2D &desiredSpacing) = 0;

    /// Returns the correspoding dimensions of the measurement, i.e., a perimeter should return 1, an area 2, a volume 3, etc,
    virtual int getMeasureDimensions() = 0;

    /// Amends coordinate taken on coordinateSpacing with the given amenderSpacing
    Vector3 amendCoordinate(const Vector3 &coordinate, double coordinateSpacing[3], const PixelSpacing2D &amenderSpacing);

protected:
    /// Given an Image and the spacing of the data where is being represented, returns the pixel spacing values that should be used for measures
    /// If image is null pixel spacing will be the same as the two first components of dataSpacing
    /// Otherwise, image's preferred spacing will be returned
    PixelSpacing2D getMeasureSpacing(Image *image, double dataSpacing[3]);
};

} // End namespace udg

#endif
