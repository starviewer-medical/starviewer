#include "standardizeduptakevalueleanbodymassformulacalculator.h"

namespace udg {

StandardizedUptakeValueLeanBodyMassFormulaCalculator::StandardizedUptakeValueLeanBodyMassFormulaCalculator()
 : StandardizedUptakeValueFormulaCalculator()
{
    m_leanBodyMassCalculator = new LeanBodyMassFormulaCalculator();
    initializeNormalizationFactorParameter();
}

StandardizedUptakeValueLeanBodyMassFormulaCalculator::~StandardizedUptakeValueLeanBodyMassFormulaCalculator()
{
    delete m_leanBodyMassCalculator;
}

bool StandardizedUptakeValueLeanBodyMassFormulaCalculator::normalizationFactorValueIsValid() const
{
    return m_leanBodyMassInGr > 0.0;
}

void StandardizedUptakeValueLeanBodyMassFormulaCalculator::gatherRequiredNormalizationFactorParameter()
{
    initializeNormalizationFactorParameter();
    
    m_leanBodyMassCalculator->setDataSource(m_tagReaderSource);
    if (m_leanBodyMassCalculator->canCompute())
    {
        m_leanBodyMassInGr = m_leanBodyMassCalculator->compute() * 1000;
    }
}

void StandardizedUptakeValueLeanBodyMassFormulaCalculator::initializeNormalizationFactorParameter()
{
    m_leanBodyMassInGr = 0.0;
}

QString StandardizedUptakeValueLeanBodyMassFormulaCalculator::getNormalizationFactorUnits() const
{
    return "g";
}

double StandardizedUptakeValueLeanBodyMassFormulaCalculator::getNormalizationFactor()
{
    return m_leanBodyMassInGr;
}

} // End namespace udg
