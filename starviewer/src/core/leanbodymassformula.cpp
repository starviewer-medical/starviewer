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

#include "leanbodymassformula.h"

#include <QtCore/qmath.h>

namespace udg {

/// Definition of teh different factors used in the formulas
const double MaleWeightMultiplier = 1.1;
const double FemaleWeightMultiplier = 1.07;
const int MaleFactor = 120;
const int FemaleFactor = 148;

LeanBodyMassFormula::LeanBodyMassFormula()
{
}

LeanBodyMassFormula::~LeanBodyMassFormula()
{
}

QString LeanBodyMassFormula::getUnits()
{
    return "Kg";
}

double LeanBodyMassFormula::computeMale(int patientsHeightInCm, int patientsWeightInKg) const
{
    return compute(patientsHeightInCm, patientsWeightInKg, MaleWeightMultiplier, MaleFactor);
}

double LeanBodyMassFormula::computeFemale(int patientsHeightInCm, int patientsWeightInKg) const
{
    return compute(patientsHeightInCm, patientsWeightInKg, FemaleWeightMultiplier, FemaleFactor);
}

double LeanBodyMassFormula::compute(int patientsHeightInCm, int patientsWeightInKg, double weightMultiplier, int weighHeightFactor) const
{
    return weightMultiplier * patientsWeightInKg - weighHeightFactor * qPow(static_cast<double>(patientsWeightInKg) / patientsHeightInCm, 2);
}

} // End namespace udg
