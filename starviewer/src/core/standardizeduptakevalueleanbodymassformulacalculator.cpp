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
