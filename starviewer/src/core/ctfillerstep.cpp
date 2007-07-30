/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ctfillerstep.h"
#include "patientfillerinput.h"
#include "logging.h"
#include "patient.h"
#include "study.h"

namespace udg {

CTFillerStep::CTFillerStep()
 : PatientFillerStep()
{
}

CTFillerStep::~CTFillerStep()
{
}

bool CTFillerStep::fill()
{
    bool ok = false;
    // processarem cadascun dels pacients que hi hagi en l'input i per cadascun totes les sèries que siguin CT
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

void CTFillerStep::processPatient( Patient *patient )
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

void CTFillerStep::processSeries( Series *series )
{
    if( series->getModality() == "CT" )
    {
        //TODO implementar el tractament específic per aquesta modalitat
        DEBUG_LOG("Cal implementar el tractament específic del CTFillerStep!");
        m_input->addLabelToSeries("CTStep", series->getInstanceUID() );
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és un CT. És de modalitat: " + series->getModality() );
    }
}

}
