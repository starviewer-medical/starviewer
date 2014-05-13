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

#ifndef UDGSTANDARDIZEDUPTAKEVALUEFORMULA_H
#define UDGSTANDARDIZEDUPTAKEVALUEFORMULA_H

#include "formula.h"

namespace udg {

/**
    Computes Standardized Uptake Value.
    The formula can be splitted in two parts, the common component and the normalization factor.
    The inherited classes will determine which normalization factor to use. The most commonly used are body weight, lean body mass and body surface area.
 */
class StandardizedUptakeValueFormula : public Formula {
public:
    StandardizedUptakeValueFormula();
    ~StandardizedUptakeValueFormula();

    /// Gets the units of the resulting formula. The final units will depend on the units of the used normalizationFactor
    QString getUnits() const;

    /// Computes the whole formula with the given normalization factor
    double compute(double activityConcentrationInBqMl, double injectedDoseInBq, double decayCorrectionFactor, double normalizationFactor = 1.0);

protected:
    /// Gets the units of the used normalization factor
    virtual QString getNormalizationFactorUnits() const = 0;
    
private:
    /// Computes the common component of the formula
    double computeCommonFormulaComponent(double activityConcentrationInBqMl, double injectedDoseInBq, double decayCorrectionFactor);

    /// Gets the units of the common component
    QString getCommonFormulaComponentUnits() const;
};

} // End namespace udg

#endif
