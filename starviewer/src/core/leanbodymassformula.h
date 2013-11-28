#ifndef UDGLEANBODYMASSFORMULA_H
#define UDGLEANBODYMASSFORMULA_H

#include "formula.h"

namespace udg {

/**
    Computes Lean Body Mass using James formula for females [1] and the Morgan formula for males [2]
    
    [1] James, W. Philip T. Research on obesity. London: Her Majesty's Stationery Office, 1976. ISBN 0114500347.

    [2] Morgan, Denis J. and Bray, Kelly M. Lean Body Mass as a Predictor of Drug Dosage: Implications for Drug Therapy.
        Clinical Pharmacokinetics. 1994, Vol. 26, 4, pp. 292-307.
 */
class LeanBodyMassFormula : public Formula {
public:
    LeanBodyMassFormula();
    ~LeanBodyMassFormula();

    static QString getUnits();

    double computeMale(int patientsHeightInCm, int patientsWeightInKg) const;
    double computeFemale(int patientsHeightInCm, int patientsWeightInKg) const;

private:
    double compute(int patientsHeightInCm, int patientsWeightInKg, double weightMultiplier, int weighHeightFactor) const;
};

} // End namespace udg

#endif
