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
