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
    double zSpacing = qAbs(Image::distance(volume->getImage(0)) - Image::distance(volume->getImage(1)));
    DEBUG_LOG(QString("Calculem el z-spacing a partir de la distància entre llesques. z-spacing llegit: %1, z-spacing calculat: %2").arg(spacing[2])
                                                                                                                                    .arg(zSpacing));

    if (!MathTools::closeEnough(zSpacing, 0.0))
    {
        spacing[2] = zSpacing;
        volume->getPixelData()->setSpacing(spacing);
    }
}

} // namespace udg
