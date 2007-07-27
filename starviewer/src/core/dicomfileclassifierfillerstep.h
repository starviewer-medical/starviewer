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

    /// Ens diu si aquella sèrie és d'imatges, kin's o presentation states
    bool isImageSeries( Series *series );
    bool isKeyImageNoteSeries( Series *series );
    bool isPresentationStateSeries( Series *series );

private:
    /// El lector de tags que anirem fent servir per analitzar les dades
    DICOMTagReader *m_dicomReader;
};

}

#endif
