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

class DICOMTagReader;
class Patient;
class Study;
class Series;

/**
    Step del PatientFiller que s'encarrega de classificar arxius de tipus DICOM en l'estructura Patient en Studis, Series i Objectes de Sèries (Imatges, KIN's,
    Presentation states, Waveforms, etc)

    L'objecte d'input (PatientFillerInput) contindrà una llista de fitxers i una estructura de llista de Patients la qual pot estar tant buida com amb contingut.
    Quan un dels arxius es detecti com a classificat (ja s'ha afegit anteriorment associat a un pacient,study uid, series uid, imatge/kin/ps) no se li omplirà cap
    mena de informació i es passarà al següent objecte. Això ens permet que un altre procés ens hagi "classificat" els arxius dins de l'estructura de
    PatientFillerInput

    TODO De moment només classifiquem imatges que és el que permet l'objecte Series.
  */
class DICOMFileClassifierFillerStep : public PatientFillerStep {
public:
    DICOMFileClassifierFillerStep();

    ~DICOMFileClassifierFillerStep();

    bool fillIndividually();

    void postProcessing() {}

    QString name()
    {
        return "DICOMFileClassifierFillerStep";
    }

private:
    /// Classifica l'arxiu que tenim al lector de tags. Presuposa que es treballa amb un lector correcte.
    void classifyFile();

private:
    /// El lector de tags que anirem fent servir per analitzar les dades
    DICOMTagReader *m_dicomReader;
};

}

#endif
