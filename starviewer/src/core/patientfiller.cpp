/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfiller.h"

namespace udg {

PatientFiller::PatientFiller() : m_input(0)
{
}

PatientFiller::~PatientFiller()
{
}

void PatientFiller::setInput( PatientFillerInput *input )
{
    m_input = input;
}

void PatientFiller::fill()
{
}

void PatientFiller::fillUntil( QString label )
{
}

void PatientFiller::registerSteps()
{
}

}
