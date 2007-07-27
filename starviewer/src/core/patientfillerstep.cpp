/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfillerstep.h"

namespace udg {

PatientFillerStep::PatientFillerStep() : m_input(0), m_priority( NormalPriority )
{
}

PatientFillerStep::~PatientFillerStep()
{
}

bool PatientFillerStep::isCandidate()
{
    // Comprovar si donats els flags de l'input i les pròpies flags és un candidat vàlid TODO per implementar
}

}
