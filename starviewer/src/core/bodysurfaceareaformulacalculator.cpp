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

#include "bodysurfaceareaformulacalculator.h"

#include "image.h"
#include "series.h"
#include "study.h"
#include "dicomtagreader.h"
#include "logging.h"

namespace udg {

BodySurfaceAreaFormulaCalculator::BodySurfaceAreaFormulaCalculator()
 : BodySurfaceAreaFormula(), FormulaCalculator()
{
    initializeParameters();
}

BodySurfaceAreaFormulaCalculator::~BodySurfaceAreaFormulaCalculator()
{
}

void BodySurfaceAreaFormulaCalculator::initializeParameters()
{
    m_patientsWeightInKg = 0;
    m_patientsHeightInCm = 0;
}

bool BodySurfaceAreaFormulaCalculator::parameterValuesAreValid() const
{
    if (m_patientsWeightInKg > 0 && m_patientsHeightInCm > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool BodySurfaceAreaFormulaCalculator::canCompute()
{
    gatherRequiredParameters();
    return parameterValuesAreValid();
}

double BodySurfaceAreaFormulaCalculator::compute()
{
    gatherRequiredParameters();
    
    return BodySurfaceAreaFormula::compute(m_patientsHeightInCm, m_patientsWeightInKg);
}

void BodySurfaceAreaFormulaCalculator::gatherRequiredParameters()
{
    initializeParameters();
    if (m_imageSource)
    {
        gatherRequiredParameters(m_imageSource);
    }
    else if (m_tagReaderSource)
    {
        gatherRequiredParameters(m_tagReaderSource);
    }
}

void BodySurfaceAreaFormulaCalculator::gatherRequiredParameters(Image *image)
{
    if (!image)
    {
        return;
    }

    Series *imageSeries = image->getParentSeries();
    if (imageSeries)
    {
        Study *imageStudy = imageSeries->getParentStudy();
        if (imageStudy)
        {
            m_patientsWeightInKg = imageStudy->getWeight();
            m_patientsHeightInCm = imageStudy->getHeight() * 100;
        }
        else
        {
            DEBUG_LOG("Body Surface Area cannot be computed with provided Image. Patient's height and weight could not be retrieved, image's Study is null.");
        }
    }
    else
    {
        DEBUG_LOG("Body Surface Area cannot be computed with provided Image. Patient's height and weight could not be retrieved, image's Series is null.");
    }
}

void BodySurfaceAreaFormulaCalculator::gatherRequiredParameters(DICOMTagReader *tagReader)
{
    if (!tagReader)
    {
        return;
    }

    m_patientsWeightInKg = tagReader->getValueAttributeAsQString(DICOMPatientWeight).toInt();
    m_patientsHeightInCm = tagReader->getValueAttributeAsQString(DICOMPatientSize).toDouble() * 100;
}

} // End namespace udg
