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

#ifndef UDGDECAYCORRECTIONFACTORFORMULACALCULATOR_H
#define UDGDECAYCORRECTIONFACTORFORMULACALCULATOR_H

#include "decaycorrectionfactorformula.h"
#include "formulacalculator.h"

namespace udg {

/**
    Computes the Decay Correction Factor using DecayCorrectionFactorFormula.
    Several fields must be present and have valid values in the provided data source.
    It could be already computed in Decay Factor tag (0054,1321). If not, it will be computed from the required tags.
*/
class DecayCorrectionFactorFormulaCalculator : public DecayCorrectionFactorFormula, public FormulaCalculator {
public:
    DecayCorrectionFactorFormulaCalculator();
    ~DecayCorrectionFactorFormulaCalculator();

    bool canCompute();
    double compute();

private:
    void initializeParameters();
    
    bool parameterValuesAreValid() const;

    void gatherRequiredParameters();
    void gatherRequiredParameters(DICOMTagReader *tagReader);

    double computeTimeLapseInSeconds() const;

private:
    /// The Decay Correction (0054,1102) is the real-world event to which images in this Series were
    /// decay corrected. If decay correction is applied, all images in the Series shall be decay corrected
    /// to the same time. The Defined Terms and definitions are:
    /// NONE = no decay correction
    /// START= acquisition start time, Acquisition Time (0008,0032)
    /// ADMIN = radiopharmaceutical administration time, Radiopharmaceutical Start Time
    /// (0018,1072)
    QString m_decayCorrection;
    
    /// Value from DICOM Tag (0054,1102) - 1C. The decay factor that was used to scale this image. Required if Decay Correction
    /// is other than NONE. If decay correction is applied, all images in the Series shall be decay corrected to the same time.
    QString m_decayFactor;
    
    /// (0018,1075) Type 3. The radionuclide half life, in seconds, that was used in the correction of this image.
    /// Contained in Radiopharmaceutical Information Sequence (0054,0016)
    double m_radionuclideHalfLifeInSeconds;
    
    /// Time lapse used in the formula.
    /// If Decay Correction = START, timeLapse = Series Time - Radiopharmaceutical Start Time (0018,1072)
    /// If Decay Correction = ADMIN, timeLapse = 0
    double m_timeLapseInSeconds;
};

} // End namespace udg

#endif
