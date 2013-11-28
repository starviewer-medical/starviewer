#include "bodysurfaceareaformula.h"

namespace udg {

BodySurfaceAreaFormula::BodySurfaceAreaFormula()
{
}

BodySurfaceAreaFormula::~BodySurfaceAreaFormula()
{
}

QString BodySurfaceAreaFormula::getUnits()
{
    return "cm2";
}

double BodySurfaceAreaFormula::compute(int patientsHeightInCm, int patientsWeightInKg)
{
    return 71.84 * pow(patientsHeightInCm, 0.725) * pow(patientsWeightInKg, 0.425);
}

} // End namespace udg
