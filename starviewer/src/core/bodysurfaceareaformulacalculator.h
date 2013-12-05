#ifndef UDGBODYSURFACEAREAFORMULACALCULATOR_H
#define UDGBODYSURFACEAREAFORMULACALCULATOR_H

#include "bodysurfaceareaformula.h"
#include "formulacalculator.h"

namespace udg {

/**
    Computes the Body Surface Area using BodySurfaceAreaFormula.
    Patient's weight and height must be present and have valid values in the provided data source
*/
class BodySurfaceAreaFormulaCalculator : public BodySurfaceAreaFormula, public FormulaCalculator {
public:
    BodySurfaceAreaFormulaCalculator();
    virtual ~BodySurfaceAreaFormulaCalculator();

    bool canCompute();
    double compute();

private:
    void initializeParameters();
    
    bool parameterValuesAreValid() const;

    void gatherRequiredParameters();
    void gatherRequiredParameters(Image *image);
    void gatherRequiredParameters(DICOMTagReader *tagReader);

private:
    int m_patientsWeightInKg;
    int m_patientsHeightInCm;
};

} // End namespace udg

#endif
