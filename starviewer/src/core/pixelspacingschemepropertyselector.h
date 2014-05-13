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

#ifndef UDGPIXELSPACINGSCHEMEPROPERTYSELECTOR_H
#define UDGPIXELSPACINGSCHEMEPROPERTYSELECTOR_H

#include <QString>

#include "pixelspacing2d.h"

namespace udg {

class Image;

/**
    Abstract class to choose or compute an specific property that depends on the pixel spacing attributes present on an image.
    Designed with the template method design pattern, execute() method implements the main algorithm and subclasses
    must implement the abstract methods to get their desired results for each case.
    Each subclass must implement its own public method to define which property is being choosen or computed and return its value.
    It should have at least an Image object passed by parameter and call execute() to run the template algorithm.
 */
class PixelSpacingSchemePropertySelector {
public:
    PixelSpacingSchemePropertySelector();
    ~PixelSpacingSchemePropertySelector();

protected:
    /// Executes the main algorithm
    void execute(const Image *image);
    
    /// These methods should be implemented by the subclasses which define the variation of results of the main algorithm
    virtual void runPixelSpacingSelector() = 0;
    virtual void runImagerPixelSpacingWithMagnificationFactorSelector() = 0;
    virtual void runImagerPixelSpacingSelector() = 0;
    virtual void runEqualPixelSpacingAndImagerPixelSpacingSelector() = 0;
    virtual void runDifferentPixelSpacingAndImagerPixelSpacingSelector() = 0;
    virtual void runNoSpacingPresentSelector() = 0;

private:
    /// Initializes properties needed for the algorithm upon the given image
    void initialize(const Image *image);

protected:
    // Properties needed for the logic of the algorithm
    QString m_modality;
    PixelSpacing2D m_pixelSpacing;
    PixelSpacing2D m_imagerPixelSpacing;
    bool m_pixelSpacingIsPresent;
    bool m_imagerPixelSpacingIsPresent;
    double m_estimatedRadiographicMagnificationFactor;
    //Image *m_image;
};

} // End namespace udg

#endif
