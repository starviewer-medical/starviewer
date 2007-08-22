/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumegeneratorstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "volumerepository.h"

namespace udg {

VolumeGeneratorStep::VolumeGeneratorStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
    // TODO afegir el label a nivell de series VolumeGeneratorStep i TemporalDimension?
    // aquest s'hauria de fer al final de tot, podríem possar-li prioritat mínima
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
        unsigned int i = 0;
        while( i < m_input->getNumberOfPatients() )
        {
            Patient *patient = m_input->getPatient( i );
            this->processPatient( patient );
            i++;
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

void VolumeGeneratorStep::processPatient( Patient *patient )
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

void VolumeGeneratorStep::processSeries( Series *series )
{
    // Podrem tenir o bé Images, o bé KINs o bé PresentationStates TODO si apliquéssim correctament lo dels labels
    // a nivell de series això no caldria
    if( isImageSeries(series) )
    {
        // TODO ara el que fem és que 1 Series equival a 1 Volume, més endavant es podrien fer un tracte més elaborat
        Volume *volume = new Volume;
        volume->setImages( series->getImages() );
        Identifier volumeID = VolumeRepository::getRepository()->addVolume( volume );
        series->setVolumeIdentifier( volumeID );
        m_input->addLabelToSeries("VolumeGeneratorStep", series );
        DEBUG_LOG("Generem un volum");
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és una sèrie d'Imatges. És de modalitat: " + series->getModality() );
    }
}

}
