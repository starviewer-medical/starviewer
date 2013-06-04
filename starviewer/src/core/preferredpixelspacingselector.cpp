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
