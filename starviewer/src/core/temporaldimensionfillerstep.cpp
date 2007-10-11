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
    QStringList list = series->getImagesPathList();
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
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " és dinàmica." );
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no és dinàmica." );
    }

    m_input->addLabelToSeries("TemporalDimensionFillerStep", series );
}

}
