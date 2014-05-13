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

#include "standardizeduptakevalueformula.h"
#include <iostream>
namespace udg {

StandardizedUptakeValueFormula::StandardizedUptakeValueFormula()
{
}

StandardizedUptakeValueFormula::~StandardizedUptakeValueFormula()
{
}

QString StandardizedUptakeValueFormula::getUnits() const
{
    return getNormalizationFactorUnits() + "/" + getCommonFormulaComponentUnits();
}

double StandardizedUptakeValueFormula::compute(double activityConcentrationInBqMl, double injectedDoseInBq, double decayCorrectionFactor, double normalizationFactor)
{
    return computeCommonFormulaComponent(activityConcentrationInBqMl, injectedDoseInBq, decayCorrectionFactor) * normalizationFactor;
}

double StandardizedUptakeValueFormula::computeCommonFormulaComponent(double activityConcentrationInBqMl, double injectedDoseInBq, double decayCorrectionFactor)
{
    return activityConcentrationInBqMl / (injectedDoseInBq * decayCorrectionFactor);
}

QString StandardizedUptakeValueFormula::getCommonFormulaComponentUnits() const
{
    return "ml";
}

} // End namespace udg
