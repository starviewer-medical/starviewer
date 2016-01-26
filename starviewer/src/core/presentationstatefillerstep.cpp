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
}

PresentationStateFillerStep::~PresentationStateFillerStep()
{
}

void PresentationStateFillerStep::processPresentationState()
{
    DICOMTagReader dicomReader;
    bool ok = false;
//     ok = dicomReader.setFile(presentationState->getPath());
    if (ok)
    {
        // TODO per implementar
        DEBUG_LOG("Pendent d'implementació!");
    }
//     else
//         DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + presentationState->getPath());
}

}
