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

#include "preferredpixelspacingselector.h"

namespace udg {

PreferredPixelSpacingSelector::PreferredPixelSpacingSelector()
 : PixelSpacingSchemePropertySelector()
{
}

PreferredPixelSpacingSelector::~PreferredPixelSpacingSelector()
{
}

PixelSpacing2D PreferredPixelSpacingSelector::getPreferredPixelSpacing(const Image *image)
{
    execute(image);
    return m_preferredSpacing;
}

void PreferredPixelSpacingSelector::runPixelSpacingSelector()
{
    m_preferredSpacing = m_pixelSpacing;
}

void PreferredPixelSpacingSelector::runImagerPixelSpacingWithMagnificationFactorSelector()
{
    // When modality is MG and we have a valid value of estimated radiographic magnification factor
    // Imager Pixel Spacing corrected by Estimated Radiographic Magnification Factor should be used
    m_preferredSpacing.setX(m_imagerPixelSpacing.x() / m_estimatedRadiographicMagnificationFactor);
    m_preferredSpacing.setY(m_imagerPixelSpacing.y() / m_estimatedRadiographicMagnificationFactor);
}

void PreferredPixelSpacingSelector::runImagerPixelSpacingSelector()
{
    m_preferredSpacing = m_imagerPixelSpacing;
}

void PreferredPixelSpacingSelector::runEqualPixelSpacingAndImagerPixelSpacingSelector()
{
    // Both values are equal, we can use any of them
    m_preferredSpacing = m_pixelSpacing;
}

void PreferredPixelSpacingSelector::runDifferentPixelSpacingAndImagerPixelSpacingSelector()
{
    // If values are different, we should use pixel spacing
    m_preferredSpacing = m_pixelSpacing;
}

void PreferredPixelSpacingSelector::runNoSpacingPresentSelector()
{
    // Neither pixel spacing nor imager pixel spacing are present,
    // Default constructed value will be returned
}

} // End namespace udg
