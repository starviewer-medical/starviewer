/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "presentationstatefillerstep.h"

namespace udg {

PresentationStateFillerStep::PresentationStateFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}


PresentationStateFillerStep::~PresentationStateFillerStep()
{
}


}
