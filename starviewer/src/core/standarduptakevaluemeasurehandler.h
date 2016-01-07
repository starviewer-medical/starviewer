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

#ifndef UDGSTANDARDUPTAKEVALUEMEASUREHANDLER_H
#define UDGSTANDARDUPTAKEVALUEMEASUREHANDLER_H

#include "dicomtagreader.h"

namespace udg {

class Image;
class StandardizedUptakeValueFormulaCalculator;

/**
    Class to handle the Standardized Uptake Value formula computing.
    It handles which is the preferred formula to use and wheter is computable or not.
    It also leds compute any of the available formulas
 */
class StandardUptakeValueMeasureHandler {
public:
    StandardUptakeValueMeasureHandler();
    ~StandardUptakeValueMeasureHandler();

    /// Enum for the different formulas that can be used
    enum FormulaType { BodyWeight, LeanBodyMass, BodySurfaceArea };

    /// Sets the image source to compute the values from
    void setImage(Image *image);
    
    /// Sets the preferred formula. It saves this configuration on user settings
    void setPreferredFormula(FormulaType formula);

    /// Gets the configured preferred formula. If none is configured, it will default to BodyWeight
    FormulaType getPreferredFormula() const;
    
    /// Returns true if the provided image has the required values to compute the preferred formula, false otherwise
    /// \sa canComputeFormula()
    bool canComputePreferredFormula();

    /// Returns true if the provided image has the required values to compute the specified formula, false otherwise
    /// \sa canComputePreferredFormula()
    bool canComputeFormula(FormulaType formula);

    /// Computes the preferred formula upon the given activity concentration value. If canComputePreferredFormula() returns false, the returned value is undefined
    /// \sa compute()
    double computePreferredFormula(double activityConcentrationValueInImageUnits);

    /// Computes the specified formula upon the given activity concentration value. If canComputedFormula() returns false for this formula, the returned value is undefined
    /// \sa computePreferredFormula()
    double compute(double activityConcentrationValueInImageUnits, FormulaType formula);

    /// Gets the label corresponding to the last computed formula. It will be updated after a successful call to compute() or computePreferredFormula()
    /// \sa getFormulaLabel(), getPreferredFormulaLabel()
    QString getComputedFormulaLabel() const;

    /// Gets the label corresponding to the preferred formula
    /// \sa getComputedFormulaLabel(), getFormulaLabel()
    QString getPreferredFormulaLabel() const;
    
    /// Gets the label corresponding to the specified formula
    /// \sa getComputedFormulaLabel(), getPreferredFormulaLabel()
    QString getFormulaLabel(FormulaType formula) const;

    /// Gets the units corresponding to the last computed formula. It will be updated after a successful call to compute() or computePreferredFormula()
    /// \sa getFormulaUnits()
    QString getComputedFormulaUnits() const;

    /// Gets the units corresponding to the specified formula. It will be updated after a successful call to compute() or computePreferredFormula()
    /// \sa getComputedFormulaUnits()
    QString getFormulaUnits(FormulaType formula) const;

private:
    /// Returns the concrete formula calculator instance correspoding to the specified formula
    StandardizedUptakeValueFormulaCalculator* getFormulaCalculator(FormulaType formula) const;

private:
    /// Image from which the needed data to compute the specified formula has to be extracted
    Image *m_image;
    
    /// DICOM tag reader, corresponding to the current provided image source
    DICOMTagReader m_tagReader;
    
    /// Label correspoding to the last computed formula
    QString m_computedFormulaLabel;

    /// Units of the last computed formula
    QString m_computedFormulaUnits;
};

} // End namespace udg

#endif
