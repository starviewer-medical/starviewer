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
    m_requiredLabelsList << "DICOMFileClassifierFillerStep" << "ImageFillerStep";
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
        QStringList requiredLabels;
        requiredLabels << "ImageFillerStep";
        QList<Series *> seriesToProcess = m_input->getSeriesWithLabels( requiredLabels );
        foreach( Series *series, seriesToProcess )
        {
            processSeries( series );
        }
        ok = true;
    }
    else
        DEBUG_LOG("No tenim input!");

    return ok;
}

void CTFillerStep::processSeries( Series *series )
{
    if( series->getModality() == "CT" )
    {
        //TODO implementar el tractament específic per aquesta modalitat
        DEBUG_LOG("Cal implementar el tractament específic del CTFillerStep!");
        m_input->addLabelToSeries("CTFillerStep", series );
    }
}

}
