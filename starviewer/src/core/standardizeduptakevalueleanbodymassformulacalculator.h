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

#ifndef UDGSTANDARDIZEDUPTAKEVALUELEANBODYMASSFORMULACALCULATOR_H
#define UDGSTANDARDIZEDUPTAKEVALUELEANBODYMASSFORMULACALCULATOR_H

#include "standardizeduptakevalueformulacalculator.h"
#include "leanbodymassformulacalculator.h"

namespace udg {

/**
    Computes Standardized Uptake Value using the Lean Body Mass normalization factor
 */
class StandardizedUptakeValueLeanBodyMassFormulaCalculator : public StandardizedUptakeValueFormulaCalculator {
public:
    StandardizedUptakeValueLeanBodyMassFormulaCalculator();
    ~StandardizedUptakeValueLeanBodyMassFormulaCalculator();

protected:
    /// Inherited pure virtual methods
    
    QString getNormalizationFactorUnits() const;
    
    bool normalizationFactorValueIsValid() const;
    double getNormalizationFactor();
    void gatherRequiredNormalizationFactorParameter();
    void initializeNormalizationFactorParameter();

protected:
    /// Used to compute the lean body mass
    LeanBodyMassFormulaCalculator *m_leanBodyMassCalculator;

private:
    /// Computed lean body mass in grams
    double m_leanBodyMassInGr;
    
};

} // End namespace udg

#endif
