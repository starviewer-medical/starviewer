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

bool KeyImageNoteFillerStep::fill()
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

void KeyImageNoteFillerStep::processPatient( Patient *patient )
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

void KeyImageNoteFillerStep::processSeries( Series *series )
{
    if( isKeyImageNoteSeries(series) )
    {
        // TODO recollir tots els elements key image note i processar-los
        m_input->addLabelToSeries("KeyImageNoteFillerStep", series->getInstanceUID() );
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és una sèrie de key image notes. És de modalitat: " + series->getModality() );
    }
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
