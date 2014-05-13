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

#include "pixelspacingschemepropertyselector.h"

#include "image.h"
#include "series.h"

namespace udg {

PixelSpacingSchemePropertySelector::PixelSpacingSchemePropertySelector()
{
    m_estimatedRadiographicMagnificationFactor = 1.0;
}

PixelSpacingSchemePropertySelector::~PixelSpacingSchemePropertySelector()
{
}

void PixelSpacingSchemePropertySelector::initialize(const Image *image)
{
    if (image)
    {
        if (image->getParentSeries())
        {
            m_modality = image->getParentSeries()->getModality();
        }

        m_pixelSpacing = image->getPixelSpacing();
        m_imagerPixelSpacing = image->getImagerPixelSpacing();
        m_estimatedRadiographicMagnificationFactor = image->getEstimatedRadiographicMagnificationFactor();
    }

    m_pixelSpacingIsPresent = m_pixelSpacing.isValid();
    m_imagerPixelSpacingIsPresent = m_imagerPixelSpacing.isValid();
}

void PixelSpacingSchemePropertySelector::execute(const Image *image)
{
    initialize(image);
    
    if (m_pixelSpacingIsPresent && !m_imagerPixelSpacingIsPresent)
    {
        runPixelSpacingSelector();
    }
    else if (!m_pixelSpacingIsPresent && m_imagerPixelSpacingIsPresent)
    {
        // Only imager pixel spacing is present
        if (m_modality == "MG" && m_estimatedRadiographicMagnificationFactor != 0.0 && m_estimatedRadiographicMagnificationFactor != 1.0)
        {
            // Imager Pixel Spacing + Estimated Radiographic Magnification Factor
            runImagerPixelSpacingWithMagnificationFactorSelector();
        }
        else
        {
            // No valid or present magnification factor
            runImagerPixelSpacingSelector();
        }
    }
    else if (m_pixelSpacingIsPresent && m_imagerPixelSpacingIsPresent)
    {
        // Both pixel spacing and imager pixel spacing are present
        if (m_pixelSpacing.isEqual(m_imagerPixelSpacing))
        {
            // Both values present and equal
            runEqualPixelSpacingAndImagerPixelSpacingSelector();
        }
        else
        {
            // Both present but with different values
            runDifferentPixelSpacingAndImagerPixelSpacingSelector();
        }
    }
    else if (!m_pixelSpacingIsPresent && !m_imagerPixelSpacingIsPresent)
    {
        // Neither pixel spacing nor imager pixel spacing are present
        runNoSpacingPresentSelector();
    }
}

} // End namespace udg
