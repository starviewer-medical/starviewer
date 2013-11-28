#ifndef UDGDECAYCORRECTIONFACTORFORMULA_H
#define UDGDECAYCORRECTIONFACTORFORMULA_H

#include "formula.h"

namespace udg {

/**
    Computes Decay Correction Factor. To be used on Standardized Uptake Value formula.
 */
class DecayCorrectionFactorFormula : public Formula {
public:
    DecayCorrectionFactorFormula();
    ~DecayCorrectionFactorFormula();

    double compute(int radionuclideHalfLifeInSeconds, int timeLapseInSeconds);
};

} // End namespace udg

#endif
