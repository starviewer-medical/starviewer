/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfillerstep.h"
#include "patientfillerinput.h"
#include "series.h"
#include "image.h"

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

bool PatientFillerStep::operator<(const PatientFillerStep &patientFillerStep) const
{
    return m_priority < patientFillerStep.getPriority();
}

bool PatientFillerStep::isImageSeries( Series *series )
{
    QStringList supportedModalitiesAsImage = Image::getSupportedModalities();

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
