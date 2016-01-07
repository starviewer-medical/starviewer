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

#ifndef UDGBODYSURFACEAREAFORMULA_H
#define UDGBODYSURFACEAREAFORMULA_H

#include "formula.h"

namespace udg {

/**
    Computes Body Surface Area using Du Bois formula[1]
    
    [1] Du Bois, Delafield and Du Bois, Eugene F. A formula to estimate the approximate surface area if height and weight be known.
        Archives of Internal Medicine. 1916, Vol. 17, 6-2, pp. 863-871.
 */
class BodySurfaceAreaFormula : public Formula {
public:
    BodySurfaceAreaFormula();
    virtual ~BodySurfaceAreaFormula();

    static QString getUnits();

    double compute(int patientsHeightInCm, int patientsWeightInKg);
};

} // End namespace udg

#endif
