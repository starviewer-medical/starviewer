/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "keyimagenotefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "dicomtagreader.h"

namespace udg {

KeyImageNoteFillerStep::KeyImageNoteFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

KeyImageNoteFillerStep::~KeyImageNoteFillerStep()
{
}

void KeyImageNoteFillerStep::processKeyImageNote()
{
    DICOMTagReader dicomReader;
    bool ok;
//     ok = dicomReader.setFile( keyImageNote->getPath() );
    if( ok )
    {
        //TODO per implementar
        DEBUG_LOG("Pendent d'implementació!");
    }
//     else
//         DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + keyImageNote->getPath() );
}

}
