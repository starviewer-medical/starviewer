#include "decaycorrectionfactorformula.h"

#include <QtCore/qmath.h>

namespace udg {

DecayCorrectionFactorFormula::DecayCorrectionFactorFormula()
{
}

DecayCorrectionFactorFormula::~DecayCorrectionFactorFormula()
{
}

double DecayCorrectionFactorFormula::compute(int radionuclideHalfLifeInSeconds, int timeLapseInSeconds)
{
    return qPow(2.0, -timeLapseInSeconds / (double)radionuclideHalfLifeInSeconds);
}

} // End namespace udg
