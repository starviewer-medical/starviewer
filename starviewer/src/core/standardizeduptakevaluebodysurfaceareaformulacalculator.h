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
