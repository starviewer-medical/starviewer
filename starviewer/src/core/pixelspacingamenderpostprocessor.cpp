#include "pixelspacingamenderpostprocessor.h"

#include "logging.h"
#include "image.h"
#include "volume.h"

namespace udg {

void PixelSpacingAmenderPostProcessor::postprocess(Volume *volume)
{
    Image *image = volume->getImage(0);
    if (!image)
    {
        return;
    }
    
    double spacing[3];
    volume->getSpacing(spacing);
    const double *pixelSpacing = image->getPixelSpacing();
    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
    {
        // TODO By default, volume spacing should be 1,1 in this case, leave it this way or setting spacing to 1,1 anyway?
        DEBUG_LOG(QString("Pixel Spacing is not present. Leaving default volume values: %1, %2").arg(spacing[0]).arg(spacing[1]));
        return;
    }
    
    bool mustAmend = false;
    for (int i = 0; i < 2; ++i)
    {
        if (!qFuzzyCompare(spacing[i], pixelSpacing[i]))
        {
            DEBUG_LOG(QString("Volume spacing and Image spacing differs at index %1: %2 != %3").arg(i).arg(spacing[i]).arg(pixelSpacing[i]));
            spacing[i] = pixelSpacing[i];
            mustAmend = true;
        }
    }
    
    if (mustAmend)
    {
        DEBUG_LOG(QString("Amending volume spacing. New spacing = %1, %2").arg(spacing[0]).arg(spacing[1]));
        volume->getPixelData()->setSpacing(spacing);
    }
}

} // namespace udg
