#include "standardizeduptakevalueformulacalculator.h"

#include "dicomtagreader.h"
#include "dicomsequenceattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceitem.h"
#include "decaycorrectionfactorformulacalculator.h"

namespace udg {

StandardizedUptakeValueFormulaCalculator::StandardizedUptakeValueFormulaCalculator()
 : StandardizedUptakeValueFormula(), FormulaCalculator()
{
    m_activityConcentrationInBqMl = 0.0;
    m_decayCorrectionCalculator = new DecayCorrectionFactorFormulaCalculator();

    initializeCommonFormulaComponentParameters();
}

StandardizedUptakeValueFormulaCalculator::~StandardizedUptakeValueFormulaCalculator()
{
    delete m_decayCorrectionCalculator;
}

void StandardizedUptakeValueFormulaCalculator::setActivityConcentrationInBqMl(double activityConcentration)
{
    m_activityConcentrationInBqMl = activityConcentration;
}

bool StandardizedUptakeValueFormulaCalculator::canCompute()
{
    gatherRequiredParameters();
    return parameterValuesAreValid();
}

double StandardizedUptakeValueFormulaCalculator::compute()
{
    gatherRequiredCommonFormulaComponentParameters();
    gatherRequiredParameters();
    return StandardizedUptakeValueFormula::compute(m_activityConcentrationInBqMl, m_injectedDoseInBq, m_decayCorrectionFactor, getNormalizationFactor());
}

void StandardizedUptakeValueFormulaCalculator::initializeCommonFormulaComponentParameters()
{
    m_injectedDoseInBq = 0.0;
    m_pixelValueUnits = "";
    m_decayCorrectionFactor = 0.0;
}

bool StandardizedUptakeValueFormulaCalculator::parameterValuesAreValid() const
{
    return commonFormulaComponentParameterValuesAreValid() && normalizationFactorValueIsValid();
}

bool StandardizedUptakeValueFormulaCalculator::commonFormulaComponentParameterValuesAreValid() const
{
    if (m_pixelValueUnits != "BQML")
    {
        return false;
    }

    if (m_injectedDoseInBq != 0.0 && m_decayCorrectionFactor != 0.0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void StandardizedUptakeValueFormulaCalculator::gatherRequiredParameters()
{
    gatherRequiredCommonFormulaComponentParameters();
    gatherRequiredNormalizationFactorParameter();
}

void StandardizedUptakeValueFormulaCalculator::gatherRequiredCommonFormulaComponentParameters()
{
    initializeCommonFormulaComponentParameters();
    // Not all the required parameters are present through image. By the moment we can only retrieve them accessing to the whole dataset
    gatherRequiredCommonFormulaComponentParameters(m_tagReaderSource);

    m_decayCorrectionCalculator->setDataSource(m_tagReaderSource);
    if (m_decayCorrectionCalculator->canCompute())
    {
        m_decayCorrectionFactor = m_decayCorrectionCalculator->compute();
    }
}

void StandardizedUptakeValueFormulaCalculator::gatherRequiredCommonFormulaComponentParameters(DICOMTagReader *tagReader)
{
    if (!tagReader)
    {
        return;
    }
    
    // This sequence is present in the PET Isotope Module (C.8.9.2 DICOM PS 3.3 2011)
    DICOMSequenceAttribute *radiopharmaceuticalInfoSequence = tagReader->getSequenceAttribute(DICOMRadiopharmaceuticalInformationSequence);
    if (radiopharmaceuticalInfoSequence)
    {
        if (!radiopharmaceuticalInfoSequence->getItems().isEmpty())
        {
            // TODO It could be more than one item!
            // Radionuclide Total Dose is a type 3 tag
            m_injectedDoseInBq = radiopharmaceuticalInfoSequence->getItems().first()->getValueAttribute(DICOMRadionuclideTotalDose)->getValueAsDouble();
        }
    }

    m_pixelValueUnits = tagReader->getValueAttributeAsQString(DICOMUnits);
}

} // End namespace udg
