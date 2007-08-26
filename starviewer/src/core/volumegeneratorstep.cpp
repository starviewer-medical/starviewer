/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumegeneratorstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "series.h"
#include "volumerepository.h"

namespace udg {

VolumeGeneratorStep::VolumeGeneratorStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep" << "TemporalDimensionFillerStep" << "ImageFillerStep";
}

VolumeGeneratorStep::~VolumeGeneratorStep()
{
}

bool VolumeGeneratorStep::fill()
{
    bool ok = false;
    // processarem cadascun dels pacients que hi hagi en l'input i per cadascun totes les sèries que siguin de tipus imatge
    if( m_input )
    {
        QStringList requiredLabels;
        requiredLabels << "TemporalDimensionFillerStep" << "ImageFillerStep"; 
        QList<Series *> seriesToProcess = m_input->getSeriesWithLabels( requiredLabels );
        foreach( Series *series, seriesToProcess )
        {
            processSeries( series );
        }
        ok = true;
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

void VolumeGeneratorStep::processSeries( Series *series )
{
    // TODO ara el que fem és que 1 Series equival a 1 Volume, més endavant es podrien fer un tracte més elaborat
    Volume *volume = new Volume;
    volume->setImages( series->getImages() );
    series->addVolume(volume);
    m_input->addLabelToSeries("VolumeGeneratorStep", series );
    DEBUG_LOG("Generem un volum");
}

}
