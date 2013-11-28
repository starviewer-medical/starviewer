#include "standardizeduptakevaluebodyweightformulacalculator.h"

#include "dicomtagreader.h"

namespace udg {

StandardizedUptakeValueBodyWeightFormulaCalculator::StandardizedUptakeValueBodyWeightFormulaCalculator()
 : StandardizedUptakeValueFormulaCalculator()
{
    initializeNormalizationFactorParameter();
}

StandardizedUptakeValueBodyWeightFormulaCalculator::~StandardizedUptakeValueBodyWeightFormulaCalculator()
{
}

void StandardizedUptakeValueBodyWeightFormulaCalculator::initializeNormalizationFactorParameter()
{
    m_bodyWeightInGr = 0;
}

bool StandardizedUptakeValueBodyWeightFormulaCalculator::normalizationFactorValueIsValid() const
{
    return m_bodyWeightInGr > 0;
}

void StandardizedUptakeValueBodyWeightFormulaCalculator::gatherRequiredNormalizationFactorParameter()
{
    initializeNormalizationFactorParameter();
    gatherRequiredNormalizationFactorParameter(m_tagReaderSource);
}

void StandardizedUptakeValueBodyWeightFormulaCalculator::gatherRequiredNormalizationFactorParameter(DICOMTagReader *tagReader)
{
    if (!tagReader)
    {
        return;
    }

    m_bodyWeightInGr = tagReader->getValueAttributeAsQString(DICOMPatientWeight).toInt() * 1000;
}

double StandardizedUptakeValueBodyWeightFormulaCalculator::getNormalizationFactor()
{
    return m_bodyWeightInGr;
}

QString StandardizedUptakeValueBodyWeightFormulaCalculator::getNormalizationFactorUnits() const
{
    return  "g";
}

} // End namespace udg
