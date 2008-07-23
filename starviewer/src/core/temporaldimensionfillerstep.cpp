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
    m_requiredLabelsList << "ImageFillerStep";
}

TemporalDimensionFillerStep::~TemporalDimensionFillerStep()
{
}

bool TemporalDimensionFillerStep::fill()
{
    bool ok = false;
    if( m_input )
    {
        QStringList requiredLabels;
        requiredLabels << "ImageFillerStep";
        QList<Series *> seriesList = m_input->getSeriesWithLabels( requiredLabels );
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

void TemporalDimensionFillerStep::processSeries( Series *series )
{
    bool found = false;
    int phases = 1;
    int slices = 0;

    if( series->getImages().at(0)->isCTLocalizer()  )
    {
        // si és un localizer no el considerarem que tingui fases
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no és dinàmica (És un CT LOCALIZER)" );
        slices = series->getImages().count();
    }
    else
    {
        QStringList list = series->getImagesPathList();
        DICOMTagReader dicomReader( list[0] );
        QString sliceLocation = dicomReader.getAttributeByName( DCM_SliceLocation );

        // l'atribut és opcional, per tant si no hi
        // és no podrem determinar si hi ha fases o no
        // TODO caldria fer servir altres mètodes alternatius
        // per determinar si tenim fases o no, en el cas que
        // no disposem del tag SliceLocation
        if( !sliceLocation.isEmpty() )
        {
            while ( !found && phases < list.count() )
            {
                dicomReader.setFile( list[phases] );
                if ( sliceLocation == dicomReader.getAttributeByName( DCM_SliceLocation  ) )
                {
                    phases++;
                }
                else
                {
                    found = true;
                }
            }
        }

        slices = list.count() / phases;
    }

    series->setNumberOfPhases( phases );
    series->setNumberOfSlicesPerPhase( slices );
    m_input->addLabelToSeries("TemporalDimensionFillerStep", series );
}

}
