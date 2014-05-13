/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
    m_activityConcentrationInImageUnits = 0.0;
    m_decayCorrectionCalculator = new DecayCorrectionFactorFormulaCalculator();
    m_philipsConversionFactorToBqMl = 0.0;

    initializeCommonFormulaComponentParameters();
}

StandardizedUptakeValueFormulaCalculator::~StandardizedUptakeValueFormulaCalculator()
{
    delete m_decayCorrectionCalculator;
}

void StandardizedUptakeValueFormulaCalculator::setActivityConcentrationInImageUnits(double activityConcentration)
{
    m_activityConcentrationInImageUnits = activityConcentration;
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

    double activityConcentrationInBqML = m_activityConcentrationInImageUnits;
    if (m_pixelValueUnits == "CNTS" && m_philipsConversionFactorToBqMl != 0.0)
    {
        activityConcentrationInBqML *= m_philipsConversionFactorToBqMl;
    }

    return StandardizedUptakeValueFormula::compute(activityConcentrationInBqML, m_injectedDoseInBq, m_decayCorrectionFactor, getNormalizationFactor());
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
    if (m_pixelValueUnits != "BQML" && m_philipsConversionFactorToBqMl == 0.0)
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

    if (m_pixelValueUnits == "CNTS")
    {
        DICOMTag philipsActivityConcentrationScaleFactor(0x7053, 0x1009);
        if (tagReader->tagExists(philipsActivityConcentrationScaleFactor))
        {
            m_philipsConversionFactorToBqMl = tagReader->getValueAttributeAsQString(philipsActivityConcentrationScaleFactor).toDouble();
        }
    }
}

} // End namespace udg
