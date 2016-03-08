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

#ifndef UDGPATIENTFILLERSTEP_H
#define UDGPATIENTFILLERSTEP_H

namespace udg {

class PatientFillerInput;
class Series;

/**
 * @brief The PatientFillerStep class is the base class of all the PatientFiller steps.
 */
class PatientFillerStep {

public:
    PatientFillerStep();
    virtual ~PatientFillerStep();

    /// Sets the patient filler input.
    void setInput(PatientFillerInput *input);

    /// Processes the current file of the input. Returns true if this step can process the file, and false otherwise.
    /// This base implementation just returns false.
    virtual bool fillIndividually();

    /// Does the final post-processing after all the steps have processed all the files. This base implementation does nothing.
    virtual void postProcessing();

protected:
    /// Returns true if the given series contains images and false otherwise.
    static bool isImageSeries(const Series *series);
    /// Returns true if the given series contains Key Image Notes and false otherwise.
    static bool isKeyImageNoteSeries(const Series *series);
    /// Returns true if the given series contains Presentation States and false otherwise.
    static bool isPresentationStateSeries(const Series *series);

protected:
    /// Information shared by all the steps.
    PatientFillerInput *m_input;

};

}

#endif
