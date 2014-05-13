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

#ifndef UDGSTANDARDIZEDUPTAKEVALUEBODYWEIGHTFORMULACALCULATOR_H
#define UDGSTANDARDIZEDUPTAKEVALUEBODYWEIGHTFORMULACALCULATOR_H

#include "standardizeduptakevalueformulacalculator.h"

namespace udg {

/**
    Computes Standardized Uptake Value using the Body Weight normalization factor
 */
class StandardizedUptakeValueBodyWeightFormulaCalculator : public StandardizedUptakeValueFormulaCalculator {
public:
    StandardizedUptakeValueBodyWeightFormulaCalculator();
    ~StandardizedUptakeValueBodyWeightFormulaCalculator();

protected:
    /// Inherited pure virtual methods
    
    QString getNormalizationFactorUnits() const;
    
    bool normalizationFactorValueIsValid() const;
    double getNormalizationFactor();
    void gatherRequiredNormalizationFactorParameter();
    void initializeNormalizationFactorParameter();

private:
    /// Gathers body weight from the given DICOMTagReader
    void gatherRequiredNormalizationFactorParameter(DICOMTagReader *tagReader);

private:
    /// Patient's body weight in grams
    int m_bodyWeightInGr;
};

} // End namespace udg

#endif
