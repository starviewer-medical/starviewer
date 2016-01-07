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

#ifndef COMPUTEZSPACINGPOSTPROCESSOR_H
#define COMPUTEZSPACINGPOSTPROCESSOR_H

#include "postprocessor.h"

namespace udg {

/**
    Postprocessador que calcula el z-spacing d'un volum a partir de la distància entre llesques.
 */
class ComputeZSpacingPostprocessor : public Postprocessor {

public:

    /// Modifica el z-spacing del volum perquè sigui igual a la distància entre les dues primeres llesques.
    virtual void postprocess(Volume *volume);

};

} // namespace udg

#endif // COMPUTEZSPACINGPOSTPROCESSOR_H
