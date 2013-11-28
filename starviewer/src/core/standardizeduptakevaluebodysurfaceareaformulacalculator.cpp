#include "standardizeduptakevaluebodysurfaceareaformulacalculator.h"

#include "bodysurfaceareaformulacalculator.h"

namespace udg {

StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::StandardizedUptakeValueBodySurfaceAreaFormulaCalculator()
 : StandardizedUptakeValueFormulaCalculator()
{
    initializeNormalizationFactorParameter();
}

StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::~StandardizedUptakeValueBodySurfaceAreaFormulaCalculator()
{
}

bool StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::normalizationFactorValueIsValid() const
{
    return m_bodySurfaceAreaInCm2 > 0.0;
}

void StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::gatherRequiredNormalizationFactorParameter()
{
    initializeNormalizationFactorParameter();

    m_bodySurfaceAreaCalculator.setDataSource(m_tagReaderSource);
    if (m_bodySurfaceAreaCalculator.canCompute())
    {
        m_bodySurfaceAreaInCm2 = m_bodySurfaceAreaCalculator.compute();
    }
}

void StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::initializeNormalizationFactorParameter()
{
    m_bodySurfaceAreaInCm2 = 0.0;
}

QString StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::getNormalizationFactorUnits() const
{
     return m_bodySurfaceAreaCalculator.getUnits();
}

double StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::getNormalizationFactor()
{
    return m_bodySurfaceAreaInCm2;
}

} // End namespace udg
