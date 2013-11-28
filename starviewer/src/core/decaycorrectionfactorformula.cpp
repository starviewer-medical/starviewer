#include "decaycorrectionfactorformula.h"

namespace udg {

DecayCorrectionFactorFormula::DecayCorrectionFactorFormula()
{
}

DecayCorrectionFactorFormula::~DecayCorrectionFactorFormula()
{
}

double DecayCorrectionFactorFormula::compute(int radionuclideHalfLifeInSeconds, int timeLapseInSeconds)
{
    return pow(2.0, -timeLapseInSeconds / radionuclideHalfLifeInSeconds);
}

} // End namespace udg
