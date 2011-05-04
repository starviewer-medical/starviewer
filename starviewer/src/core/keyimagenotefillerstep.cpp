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
    bool ok = false;
//     ok = dicomReader.setFile(keyImageNote->getPath());
    if (ok)
    {
        //TODO per implementar
        DEBUG_LOG("Pendent d'implementació!");
    }
//     else
//         DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + keyImageNote->getPath());
}

}
