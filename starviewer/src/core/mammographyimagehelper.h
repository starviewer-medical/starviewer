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
