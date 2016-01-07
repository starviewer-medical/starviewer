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

#ifndef MAMMOGRAPHYIMAGEHELPER_H
#define MAMMOGRAPHYIMAGEHELPER_H

#include <QString>

#include "patientorientation.h"

namespace udg {

class Image;

class MammographyImageHelper
{
public:
    MammographyImageHelper();
    virtual ~MammographyImageHelper();

    /// Translates View Code Meaning from image to the corresponding mammography projection label.
    const QString getMammographyProjectionLabel(Image *image);

    /// Returns true if we can treat the image as an standard mamography, false otherwise
    bool isStandardMammographyImage(Image *image);

    /// Returns image orientation according to the preferred presentation depending on its attributes
    const PatientOrientation getImageOrientationPresentation(Image *image);

protected:
    /// Returns a list of exceptions when the auto orientation has not be applied
    virtual const QStringList getMammographyAutoOrientationExceptions();
};

}

#endif // MAMMOGRAPHYIMAGEHELPER_H
