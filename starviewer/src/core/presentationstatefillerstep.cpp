#include "presentationstatefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "dicomtagreader.h"

namespace udg {

PresentationStateFillerStep::PresentationStateFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

PresentationStateFillerStep::~PresentationStateFillerStep()
{
}

void PresentationStateFillerStep::processPresentationState()
{
    DICOMTagReader dicomReader;
    bool ok = false;
//     ok = dicomReader.setFile( presentationState->getPath() );
    if( ok )
    {
        //TODO per implementar
        DEBUG_LOG("Pendent d'implementació!");
    }
//     else
//         DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + presentationState->getPath() );
}

}
