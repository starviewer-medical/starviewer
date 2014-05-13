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

#include "standardizeduptakevaluebodysurfaceareaformulacalculator.h"

#include "bodysurfaceareaformulacalculator.h"

namespace udg {

StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::StandardizedUptakeValueBodySurfaceAreaFormulaCalculator()
 : StandardizedUptakeValueFormulaCalculator()
{
    m_bodySurfaceAreaCalculator = new BodySurfaceAreaFormulaCalculator();
    initializeNormalizationFactorParameter();
}

StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::~StandardizedUptakeValueBodySurfaceAreaFormulaCalculator()
{
    delete m_bodySurfaceAreaCalculator;
}

bool StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::normalizationFactorValueIsValid() const
{
    return m_bodySurfaceAreaInCm2 > 0.0;
}

void StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::gatherRequiredNormalizationFactorParameter()
{
    initializeNormalizationFactorParameter();

    m_bodySurfaceAreaCalculator->setDataSource(m_tagReaderSource);
    if (m_bodySurfaceAreaCalculator->canCompute())
    {
        m_bodySurfaceAreaInCm2 = m_bodySurfaceAreaCalculator->compute();
    }
}

void StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::initializeNormalizationFactorParameter()
{
    m_bodySurfaceAreaInCm2 = 0.0;
}

QString StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::getNormalizationFactorUnits() const
{
     return m_bodySurfaceAreaCalculator->getUnits();
}

double StandardizedUptakeValueBodySurfaceAreaFormulaCalculator::getNormalizationFactor()
{
    return m_bodySurfaceAreaInCm2;
}

} // End namespace udg
