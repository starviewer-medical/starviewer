#ifndef UDGLEANBODYMASSFORMULACALCULATOR_H
#define UDGLEANBODYMASSFORMULACALCULATOR_H

#include "leanbodymassformula.h"
#include "formulacalculator.h"

namespace udg {

/**
    Computes the Lean Body Mass using LeanBodyMassFormula.
    Patient's sex, weight and height must be present and have valid values in the provided data source
*/
class LeanBodyMassFormulaCalculator : public LeanBodyMassFormula, public FormulaCalculator {
public:
    LeanBodyMassFormulaCalculator();
    ~LeanBodyMassFormulaCalculator();

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
    QString m_patientsSex;
};

} // End namespace udg

#endif
