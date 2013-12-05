#include "testingdecaycorrectionfactorformulacalculator.h"

TestingDecayCorrectionFactorFormulaCalculator::TestingDecayCorrectionFactorFormulaCalculator()
{
}

bool TestingDecayCorrectionFactorFormulaCalculator::canCompute()
{
    return m_canCompute;
}

double TestingDecayCorrectionFactorFormulaCalculator::compute()
{
    return m_computeValue;
}
