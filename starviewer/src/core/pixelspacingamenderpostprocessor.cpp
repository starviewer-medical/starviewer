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
    PixelSpacing2D pixelSpacing = image->getPixelSpacing();
    if (!pixelSpacing.isValid())
    {
        // TODO By default, volume spacing should be 1,1 in this case, leave it this way or setting spacing to 1,1 anyway?
        DEBUG_LOG(QString("Pixel Spacing is not present. Leaving default volume values: %1, %2").arg(spacing[0]).arg(spacing[1]));
        return;
    }
    
    PixelSpacing2D volumeSpacing(spacing[0], spacing[1]);
    bool mustAmend = false;
    
    if (!pixelSpacing.isEqual(volumeSpacing))
    {
        spacing[0] = pixelSpacing.x();
        spacing[1] = pixelSpacing.y();
        mustAmend = true;
    }
    
    if (mustAmend)
    {
        DEBUG_LOG(QString("Amending volume spacing. New spacing = %1, %2").arg(spacing[0]).arg(spacing[1]));
        volume->getPixelData()->setSpacing(spacing);
    }
}

} // namespace udg
