#ifndef UDGPREFERREDPIXELSPACINGSELECTOR_H
#define UDGPREFERREDPIXELSPACINGSELECTOR_H

#include "pixelspacingschemepropertyselector.h"

namespace udg {

/**
    Class to choose the preferred pixel spacing of an image.
    Inherits template algorithm from PixelSpacingSchemePropertySelector and implements its own variant to get the desired result.
 */
class PreferredPixelSpacingSelector : public PixelSpacingSchemePropertySelector {
public:
    PreferredPixelSpacingSelector();
    ~PreferredPixelSpacingSelector();

    PixelSpacing2D getPreferredPixelSpacing(const Image *image);

protected:
    void runPixelSpacingSelector();
    void runImagerPixelSpacingWithMagnificationFactorSelector();
    void runImagerPixelSpacingSelector();
    void runEqualPixelSpacingAndImagerPixelSpacingSelector();
    void runDifferentPixelSpacingAndImagerPixelSpacingSelector();
    void runNoSpacingPresentSelector();

private:
    PixelSpacing2D m_preferredSpacing;
};

} // End namespace udg

#endif
