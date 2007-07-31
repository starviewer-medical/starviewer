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
class Image;

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

    bool fill();

private:
    /// Donat un arxiu el classifica dins de pacient. Retorna fals si no el pot classificar, és a dir, no és un arxiu que es pugui parsejar com a DICOM, cert altrament.
    bool classifyFile( QString file );

    /// Retorna el pacient que cumpleixi un dels dos paràmetres. \TODO quin seria el criteri adequat per identificar un mateix pacient? només pel nom? només per l'ID? la concatenació de nom i d'ID?
    Patient *getPatient( QString patientName, QString patientID );

    /// Crea un nou pacient/estudi/series/imatge a partir de les dades del fitxer que s'està tractant
    Patient *createPatient();
    Study *createStudy();
    Series *createSeries();
    Image *createImage();

private:
    /// El lector de tags que anirem fent servir per analitzar les dades
    DICOMTagReader *m_dicomReader;
};

}

#endif
