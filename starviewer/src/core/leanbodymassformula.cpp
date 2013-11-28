#include "leanbodymassformula.h"

namespace udg {

/// Definition of teh different factors used in the formulas
const double MaleWeightMultiplier = 1.1;
const double FemaleWeightMultiplier = 1.07;
const int MaleFactor = 120;
const int FemaleFactor = 148;

LeanBodyMassFormula::LeanBodyMassFormula()
{
}

LeanBodyMassFormula::~LeanBodyMassFormula()
{
}

QString LeanBodyMassFormula::getUnits()
{
    return "Kg";
}

double LeanBodyMassFormula::computeMale(int patientsHeightInCm, int patientsWeightInKg) const
{
    return compute(patientsHeightInCm, patientsWeightInKg, MaleWeightMultiplier, MaleFactor);
}

double LeanBodyMassFormula::computeFemale(int patientsHeightInCm, int patientsWeightInKg) const
{
    return compute(patientsHeightInCm, patientsWeightInKg, FemaleWeightMultiplier, FemaleFactor);
}

double LeanBodyMassFormula::compute(int patientsHeightInCm, int patientsWeightInKg, double weightMultiplier, int weighHeightFactor) const
{
    return weightMultiplier * patientsWeightInKg - weighHeightFactor * pow(static_cast<double>(patientsWeightInKg) / patientsHeightInCm, 2);
}

} // End namespace udg
