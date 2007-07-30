/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCTFILLERSTEP_H
#define UDGCTFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;

/**
PatientFillerStep que es dedica a omplir la informació específica de CT de les series dels Pacients que es contenen en un PatientFillerInput

El prerequisit per poder processar correctament l'input per aquest mòdul és haver passat per la classificació DICOM i pel mòdul d'imatge.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CTFillerStep : public PatientFillerStep
{
public:
    CTFillerStep();

    ~CTFillerStep();

    bool fill();

private:
    /// Donat un pacient, processa la seva informació
    void processPatient( Patient *patient );

    /// Donada una serie, processa la seva informació si es tracta d'un CT
    void processSeries( Series *series );
};

}

#endif
