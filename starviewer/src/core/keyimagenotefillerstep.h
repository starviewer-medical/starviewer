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

#ifndef UDGKEYIMAGENOTEFILLERSTEP_H
#define UDGKEYIMAGENOTEFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;

/**
    Mòdul que s'encarrega d'omplir la informació d'objectes KIN. Un dels seus prerequisits serà que s'hagi superat el mòdul DICOMFileClassifierFillerStep.
    Les Series a processar han de de ser de modalitat KO.
  */
class KeyImageNoteFillerStep : public PatientFillerStep {
public:
    KeyImageNoteFillerStep();

    ~KeyImageNoteFillerStep();

    bool fillIndividually()
    {
        return false;
    };

    void postProcessing(){};

    QString name()
    {
        return "KeyImageNoteFillerStep";
    }

private:
    // TODO mètode per implementar
    void processKeyImageNote();
};

}

#endif
