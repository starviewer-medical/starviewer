/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMFILECLASSIFIERFILLERSTEP_H
#define UDGDICOMFILECLASSIFIERFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class DICOMTagReader;
class Patient;
class Study;
class Series;

/**
Step del PatientFiller que s'encarrega de classificar arxius de tipus DICOM en l'estructura Patient en Studis, Series i Objectes de Sèries (Imatges, KIN's, Presentation states, Waveforms, etc)

L'objecte d'input (PatientFillerInput) contindrà una llista de fitxers i una estructura de llista de Patients la qual pot estar tant buida com amb contingut. Quan un dels arxius es detecti com a classificat (ja s'ha afegit anteriorment associat a un pacient,study uid, series uid, imatge/kin/ps) no se li omplirà cap mena de informació i es passarà al següent objecte. Això ens permet que un altre procés ens hagi "classificat" els arxius dins de l'estructura de PatientFillerInput

TODO De moment només classifiquem imatges que és el que permet l'objecte Series.


	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMFileClassifierFillerStep : public PatientFillerStep
{
public:
    DICOMFileClassifierFillerStep();

    ~DICOMFileClassifierFillerStep();

    bool fillIndividually();

    void postProcessing() {}

    QString name() {  return "DICOMFileClassifierFillerStep";  }

private:
    /// Classifica l'arxiu que tenim al lector de tags. Presuposa que es treballa amb un lector correcte.
    void classifyFile();

private:
    /// El lector de tags que anirem fent servir per analitzar les dades
    DICOMTagReader *m_dicomReader;
};

}

#endif
