#include "computezspacingpostprocessor.h"

#include "image.h"
#include "logging.h"
#include "mathtools.h"
#include "volume.h"

namespace udg {

void ComputeZSpacingPostprocessor::postprocess(Volume *volume)
{
    if (volume->getNumberOfSlicesPerPhase() < 2)
    {
        return;
    }

    double spacing[3];
    volume->getSpacing(spacing);
    double zSpacing = abs(Image::distance(volume->getImage(0)) - Image::distance(volume->getImage(1)));
    DEBUG_LOG(QString("Calculem el z-spacing a partir de la distància entre llesques. z-spacing llegit: %1, z-spacing calculat: %2").arg(spacing[2])
                                                                                                                                    .arg(zSpacing));

    if (!MathTools::closeEnough(zSpacing, 0.0))
    {
        spacing[2] = zSpacing;
        volume->getPixelData()->setSpacing(spacing);
    }
}

} // namespace udg
