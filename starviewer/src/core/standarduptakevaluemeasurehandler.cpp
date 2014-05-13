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

#include "standarduptakevaluemeasurehandler.h"

#include "image.h"

#include "standardizeduptakevalueformulacalculator.h"
#include "standardizeduptakevaluebodysurfaceareaformulacalculator.h"
#include "standardizeduptakevaluebodyweightformulacalculator.h"
#include "standardizeduptakevalueleanbodymassformulacalculator.h"

#include "coresettings.h"

namespace udg {

StandardUptakeValueMeasureHandler::StandardUptakeValueMeasureHandler()
{
    m_image = 0;
}

StandardUptakeValueMeasureHandler::~StandardUptakeValueMeasureHandler()
{
}

void StandardUptakeValueMeasureHandler::setImage(Image *image)
{
    m_image = image;

    if (m_image)
    {
        m_tagReader.setFile(m_image->getPath());
    }
}

void StandardUptakeValueMeasureHandler::setPreferredFormula(FormulaType formula)
{
    QString settingValue;
    switch (formula)
    {
        case BodyWeight:
            settingValue = "BodyWeight";
            break;

        case LeanBodyMass:
            settingValue = "LeanBodyMass";
            break;

        case BodySurfaceArea:
            settingValue = "BodySurfaceArea";
            break;
    }

    Settings settings;
    settings.setValue(CoreSettings::SUVMeasurementNormalizationType, settingValue);
}

StandardUptakeValueMeasureHandler::FormulaType StandardUptakeValueMeasureHandler::getPreferredFormula() const
{
    FormulaType formula;
    Settings settings;
    QString suvType = settings.getValue(CoreSettings::SUVMeasurementNormalizationType).toString();

    if (suvType == "BodyWeight")
    {
        formula = BodyWeight;
    }
    else if (suvType == "LeanBodyMass")
    {
        formula = LeanBodyMass;
    }
    else if (suvType == "BodySurfaceArea")
    {
        formula = BodySurfaceArea;
    }
    else
    {
        // If no valid value is found, we set the default
        formula = BodyWeight;
    }

    return formula;
}

bool StandardUptakeValueMeasureHandler::canComputePreferredFormula()
{
    return canComputeFormula(getPreferredFormula());
}

bool StandardUptakeValueMeasureHandler::canComputeFormula(FormulaType formula)
{
    if (!m_image)
    {
        return false;
    }
    
    StandardizedUptakeValueFormulaCalculator *suvCalculator = getFormulaCalculator(formula);

    bool canCompute = false;
    if (suvCalculator)
    {
        suvCalculator->setDataSource(&m_tagReader);
        canCompute = suvCalculator->canCompute();
    }
    delete suvCalculator;

    return canCompute;
}

double StandardUptakeValueMeasureHandler::computePreferredFormula(double activityConcentrationValueInImageUnits)
{
    return compute(activityConcentrationValueInImageUnits, getPreferredFormula());
}

double StandardUptakeValueMeasureHandler::compute(double activityConcentrationValueInImageUnits, FormulaType formula)
{
    StandardizedUptakeValueFormulaCalculator *suvCalculator = getFormulaCalculator(formula);
    m_computedFormulaLabel = getFormulaLabel(formula);
    m_computedFormulaUnits = "";

    // TODO If the preferred calculator cannot be computed, make a fallback to test the other available calculators?    
    double computedValue = 0.0;
    if (suvCalculator)
    {
        suvCalculator->setActivityConcentrationInImageUnits(activityConcentrationValueInImageUnits);
        suvCalculator->setDataSource(&m_tagReader);
        computedValue = suvCalculator->compute();

        m_computedFormulaUnits = suvCalculator->getUnits();
    }
    delete suvCalculator;

    return computedValue;
}

QString StandardUptakeValueMeasureHandler::getComputedFormulaLabel() const
{
    return m_computedFormulaLabel;
}

QString StandardUptakeValueMeasureHandler::getPreferredFormulaLabel() const
{
    return getFormulaLabel(getPreferredFormula());
}

QString StandardUptakeValueMeasureHandler::getFormulaLabel(FormulaType formula) const
{
    QString label;
    
    switch (formula)
    {
        case BodyWeight:
            label = "bw";
            break;

        case LeanBodyMass:
            label = "lbm";
            break;

        case BodySurfaceArea:
            label = "bsa";
            break;
    }
    
    return label;
}

QString StandardUptakeValueMeasureHandler::getComputedFormulaUnits() const
{
    return m_computedFormulaUnits;
}

QString StandardUptakeValueMeasureHandler::getFormulaUnits(FormulaType formula) const
{
    StandardizedUptakeValueFormulaCalculator *suvCalculator = getFormulaCalculator(formula);

    QString units;
    if (suvCalculator)
    {
        units = suvCalculator->getUnits();
    }
    delete suvCalculator;
    
    return units;
}

StandardizedUptakeValueFormulaCalculator* StandardUptakeValueMeasureHandler::getFormulaCalculator(FormulaType formula) const
{
    StandardizedUptakeValueFormulaCalculator *suvCalculator = 0;
    
    switch (formula)
    {
        case BodyWeight:
            suvCalculator = new StandardizedUptakeValueBodyWeightFormulaCalculator;
            break;

        case LeanBodyMass:
            suvCalculator = new StandardizedUptakeValueLeanBodyMassFormulaCalculator;
            break;

        case BodySurfaceArea:
            suvCalculator = new StandardizedUptakeValueBodySurfaceAreaFormulaCalculator;
            break;
    }

    return suvCalculator;
}

} // End namespace udg
