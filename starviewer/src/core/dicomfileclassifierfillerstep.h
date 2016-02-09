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

#ifndef UDGDICOMFILECLASSIFIERFILLERSTEP_H
#define UDGDICOMFILECLASSIFIERFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

/**
 * @brief The DICOMFileClassifierFillerStep class creates patients, studies and series from DICOM files.
 */
class DICOMFileClassifierFillerStep : public PatientFillerStep {

public:
    DICOMFileClassifierFillerStep();
    virtual ~DICOMFileClassifierFillerStep();

    /// Creates a patient, a study and a series from the current DICOM file in the input, if they aren't already crated, and returns true.
    virtual bool fillIndividually() override;

};

}

#endif
