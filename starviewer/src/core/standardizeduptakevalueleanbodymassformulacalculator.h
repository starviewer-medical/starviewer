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

private:
    /// Computed lean body mass in grams
    double m_leanBodyMassInGr;
    
    /// Used to compute the lean body mass
    LeanBodyMassFormulaCalculator m_leanBodyMassCalculator;
};

} // End namespace udg

#endif
