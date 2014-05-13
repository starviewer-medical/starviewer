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
