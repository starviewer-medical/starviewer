#include "standardizeduptakevalueformula.h"

namespace udg {

StandardizedUptakeValueFormula::StandardizedUptakeValueFormula()
{
}

StandardizedUptakeValueFormula::~StandardizedUptakeValueFormula()
{
}

QString StandardizedUptakeValueFormula::getUnits() const
{
    return getNormalizationFactorUnits() + "/" + getCommonFormulaComponentUnits();
}

double StandardizedUptakeValueFormula::compute(double activityConcentrationInBqMl, int injectedDoseInBq, int decayCorrectionFactor, double normalizationFactor)
{
    return computeCommonFormulaComponent(activityConcentrationInBqMl, injectedDoseInBq, decayCorrectionFactor) * normalizationFactor;
}

double StandardizedUptakeValueFormula::computeCommonFormulaComponent(double activityConcentrationInBqMl, int injectedDoseInBq, int decayCorrectionFactor)
{
    return activityConcentrationInBqMl / (injectedDoseInBq * decayCorrectionFactor);
}

QString StandardizedUptakeValueFormula::getCommonFormulaComponentUnits() const
{
    return "ml";
}

} // End namespace udg
