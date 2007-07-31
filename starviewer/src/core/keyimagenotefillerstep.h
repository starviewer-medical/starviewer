/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTEFILLERSTEP_H
#define UDGKEYIMAGENOTEFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

/**
Mòdul que s'encarrega d'omplir la informació d'objectes KIN. Un dels seus prerequisits serà que s'hagi superat el mòdul DICOMFileClassifierFillerStep. Les Series a processar han de de ser de modalitat KO.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteFillerStep : public PatientFillerStep
{
public:
    KeyImageNoteFillerStep();

    ~KeyImageNoteFillerStep();

};

}

#endif
