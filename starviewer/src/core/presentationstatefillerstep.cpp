/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

bool PresentationStateFillerStep::fill()
{
    bool ok = false;
    // processarem cadascun dels pacients que hi hagi en l'input i per cadascun totes les sèries que siguin de tipus imatge
    if( m_input )
    {
        unsigned int i = 0;
        while( i < m_input->getNumberOfPatients() )
        {
            Patient *patient = m_input->getPatient( i );
            this->processPatient( patient );
            i++;
        }
    }
    else
        DEBUG_LOG("No tenim input!");

    return ok;
}

void PresentationStateFillerStep::processPatient( Patient *patient )
{
    QList<Study *> studyList = patient->getStudies();
    foreach( Study *study, studyList )
    {
        QList<Series *> seriesList = study->getSeries();
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
    }
}

void PresentationStateFillerStep::processSeries( Series *series )
{
    if( isPresentationStateSeries(series) )
    {
        // TODO recopilar tots els elements presentation state i processar-los
        m_input->addLabelToSeries("PresentationStateFillerStep", series->getInstanceUID() );
    }
}

void PresentationStateFillerStep::processPresentationState()
{
    DICOMTagReader dicomReader;
    bool ok;
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
