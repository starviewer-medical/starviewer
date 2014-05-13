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

#ifndef UDGSTANDARDIZEDUPTAKEVALUEBODYSURFACEAREAFORMULACALCULATOR_H
#define UDGSTANDARDIZEDUPTAKEVALUEBODYSURFACEAREAFORMULACALCULATOR_H

#include "standardizeduptakevalueformulacalculator.h"
#include "bodysurfaceareaformulacalculator.h"

namespace udg {

/**
    Computes Standardized Uptake Value using the Body Surface Area normalization factor
 */
class StandardizedUptakeValueBodySurfaceAreaFormulaCalculator : public StandardizedUptakeValueFormulaCalculator {
public:
    StandardizedUptakeValueBodySurfaceAreaFormulaCalculator();
    ~StandardizedUptakeValueBodySurfaceAreaFormulaCalculator();

protected:
    /// Inherited pure virtual methods
    
    QString getNormalizationFactorUnits() const;
    
    bool normalizationFactorValueIsValid() const;
    double getNormalizationFactor();
    void gatherRequiredNormalizationFactorParameter();
    void initializeNormalizationFactorParameter();

protected:
    /// Used to compute the body surface area
    BodySurfaceAreaFormulaCalculator *m_bodySurfaceAreaCalculator;

private:
    /// Computed body surface area in squared cm
    double m_bodySurfaceAreaInCm2;

};

} // End namespace udg

#endif
