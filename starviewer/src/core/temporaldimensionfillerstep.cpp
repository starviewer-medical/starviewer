/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "temporaldimensionfillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include <QStringList>

namespace udg {

TemporalDimensionFillerStep::TemporalDimensionFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

TemporalDimensionFillerStep::~TemporalDimensionFillerStep()
{
}

bool TemporalDimensionFillerStep::fill()
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

void TemporalDimensionFillerStep::processPatient( Patient *patient )
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

void TemporalDimensionFillerStep::processSeries( Series *series )
{
    if( isImageSeries(series) )
    {
        bool found = false;
        int phases = 1;
        int slices = 0;
        QStringList list = series->getFilesPathList();
        DICOMTagReader dicomReader( list[0] );
        QString sliceLocation = dicomReader.getAttributeByName( DCM_SliceLocation );

        while ( !found && phases < list.count() )
        {
            dicomReader.setFile( list[phases] );
            if ( sliceLocation == dicomReader.getAttributeByName( DCM_SliceLocation ) )
            {
                phases++;
            }
            else
            {
                found = true;
            }
        }

        slices = list.count() / phases;

        series->setNumberOfPhases( phases );
        series->setNumberOfSlicesPerPhase( slices );
        if ( phases > 1 ) // és dinàmic
        {
            m_input->addLabelToSeries("TemporalDimensionFillerStep", series );
            DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " és dinàmica." );
        }
        else
        {
            DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no és dinàmica." );
        }
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és una sèrie d'Imatges. És de modalitat: " + series->getModality() );
    }
}

}
