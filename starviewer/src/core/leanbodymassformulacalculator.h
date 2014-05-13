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

#ifndef UDGLEANBODYMASSFORMULACALCULATOR_H
#define UDGLEANBODYMASSFORMULACALCULATOR_H

#include "leanbodymassformula.h"
#include "formulacalculator.h"

namespace udg {

/**
    Computes the Lean Body Mass using LeanBodyMassFormula.
    Patient's sex, weight and height must be present and have valid values in the provided data source
*/
class LeanBodyMassFormulaCalculator : public LeanBodyMassFormula, public FormulaCalculator {
public:
    LeanBodyMassFormulaCalculator();
    ~LeanBodyMassFormulaCalculator();

    bool canCompute();
    double compute();

private:
    void initializeParameters();
    
    bool parameterValuesAreValid() const;

    void gatherRequiredParameters();
    void gatherRequiredParameters(Image *image);
    void gatherRequiredParameters(DICOMTagReader *tagReader);

private:
    int m_patientsWeightInKg;
    int m_patientsHeightInCm;
    QString m_patientsSex;
};

} // End namespace udg

#endif
