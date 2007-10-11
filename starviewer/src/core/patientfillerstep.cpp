/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfillerstep.h"
#include "patientfillerinput.h"
#include "series.h"

namespace udg {

PatientFillerStep::PatientFillerStep() : m_input(0), m_priority( NormalPriority )
{
}

PatientFillerStep::~PatientFillerStep()
{
}

void PatientFillerStep::setInput( PatientFillerInput *input )
{
    m_input = input;
}

bool PatientFillerStep::isCandidate()
{
    // Comprovar si donats els flags de l'input i les pròpies flags és un candidat vàlid TODO per implementar
}

bool PatientFillerStep::operator<(const PatientFillerStep &patientFillerStep) const
{
    return m_priority < patientFillerStep.getPriority();
}

bool PatientFillerStep::isImageSeries( Series *series )
{
    // TODO aquí caldria especificar quines són les modalitats que acceptem com a imatges
    QStringList supportedModalitiesAsImage;
    supportedModalitiesAsImage << "CT" << "MR" << "US" << "DX" << "MG" << "OT" << "RF" << "CR" << "ES" << "NM" << "DT" << "SC" << "PT" << "XA";
    return supportedModalitiesAsImage.contains( series->getModality() );
}

bool PatientFillerStep::isKeyImageNoteSeries( Series *series )
{
    return series->getModality() == "KO";
}

bool PatientFillerStep::isPresentationStateSeries( Series *series )
{
    return series->getModality() == "PR";
}

}
