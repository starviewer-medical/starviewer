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
    PixelSpacing2D pixelSpacing = image->getPreferredPixelSpacing();
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
        DEBUG_LOG(QString("Volume spacing (%1, %2) and preferred Image spacing (%3, %4) are not equal:")
            .arg(volumeSpacing.x()).arg(volumeSpacing.y()).arg(pixelSpacing.x()).arg(pixelSpacing.y()));
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
