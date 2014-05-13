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

#ifndef UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H
#define UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H

#include "postprocessor.h"

namespace udg {

/**
    Post processor that amends pixel spacing if the values in Volume differ from the values in its images
 */
class PixelSpacingAmenderPostProcessor : public Postprocessor {
public:
    /// Amends volume's X, Y pixel spacing in case it differs from its images
    virtual void postprocess(Volume *volume);
};

} // namespace udg

#endif // UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H
