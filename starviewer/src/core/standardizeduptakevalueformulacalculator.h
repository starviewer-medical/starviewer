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

#ifndef UDGSTANDARDIZEDUPTAKEVALUEBASEFORMULACALCULATOR_H
#define UDGSTANDARDIZEDUPTAKEVALUEBASEFORMULACALCULATOR_H

#include "formulacalculator.h"
#include "standardizeduptakevalueformula.h"
#include "decaycorrectionfactorformulacalculator.h"

namespace udg {

/**
    Base calculator for StandardizedUptakeValue.

    Subclasses must define which normalization factor to use.
 */
class StandardizedUptakeValueFormulaCalculator : public StandardizedUptakeValueFormula, public FormulaCalculator {
public:
    StandardizedUptakeValueFormulaCalculator();
    virtual ~StandardizedUptakeValueFormulaCalculator();

    /// Value upon the SUV value will be computed
    void setActivityConcentrationInImageUnits(double activityConcentration);
    
    /// Returns true if it has the proper data and parameters to compute SUV, false otherwise
    bool canCompute();
    
    /// Computes SUV upon given data. Activity concentration parameter must be supplied through setActivityConcentrationInBqMl().
    /// If canCompute() returns false, the resulting value is undefined and invalid
    double compute();

protected:
    /// Checks if normalization factor value is valid
    virtual bool normalizationFactorValueIsValid() const = 0;
    
    /// Gets the normalization factor value. Corresponding values must be gathered previously to have a valid value
    virtual double getNormalizationFactor() = 0;

    /// Gathers the corresponding value of the normalization factor parameter
    virtual void gatherRequiredNormalizationFactorParameter() = 0;
    
    /// Must be implemented by the subclass to initialize the normalization factor parameter
    virtual void initializeNormalizationFactorParameter() = 0;

private:
    /// Gathers the required parameters, the ones from common formula component, and the needed for the normalization factor
    void gatherRequiredParameters();
    
    /// Checks if all the current gathered parameter values (common and normalization) are valid to compute the formula
    bool parameterValuesAreValid() const;
    
    /// Methods to initialize, validate and gather the parameters of the common formula component
    void initializeCommonFormulaComponentParameters();
    bool commonFormulaComponentParameterValuesAreValid() const;
    void gatherRequiredCommonFormulaComponentParameters();
    void gatherRequiredCommonFormulaComponentParameters(DICOMTagReader *tagReader);

protected:
    /// Used to compute the decay
    DecayCorrectionFactorFormulaCalculator *m_decayCorrectionCalculator;

private:
    /// External given value to compute SUV from
    double m_activityConcentrationInImageUnits;

    /// Radiopharmaceutical dose administered to patient at administration time in becquerels (Bq)
    /// Extracted from Radionuclide Total Dose (0018,1074) tag
    double m_injectedDoseInBq;

    /// The pixel value units of the PET series (after rescale slope/intercept is applied). It should be BQML.
    /// Extracted from Units (0054,1001) tag (Type 1 for PET series).
    QString m_pixelValueUnits;

    /// The decay correction factor. Will be computed with DecayCorrectionFactorFormulaCalculator.
    double m_decayCorrectionFactor;

    /// Philips' private tag for Activity Concentration Scale Factor
    double m_philipsConversionFactorToBqMl;
};

} // End namespace udg

#endif
