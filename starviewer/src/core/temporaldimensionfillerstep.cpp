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

    // si és un localizer no el considerarem que tingui fases
    if (dicomReader.tagExists( DCM_ImageType ))
    {
        // aquest valor és de tipus 3 al mòdul General Image, però consta com a tipus 1 a
        // gairebé totes les modalitats. Només consta com a tipus 2 per la modalitat US
        QString value = dicomReader.getAttributeByName( DCM_ImageType );
        if( series->getModality() == "CT" ) // en el cas del CT ens interessa saber si és localizer
        {
            QStringList valueList = value.split( "\\" );
            if( valueList.count() >= 3 )
            {
                if( valueList.at(2) == "LOCALIZER" )
                {
                    DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no és dinàmica (És un CT LOCALIZER)" );
                    slices = series->getImages().count();
                }
            }
            else
            {
                // TODO aquesta comprovació s'ha afegit perquè hem trobat un cas en que aquestes dades apareixen incoherents
                // tot i així, lo seu seria disposar d'alguna eina que comprovés si les dades són consistents o no.
                DEBUG_LOG( "ERROR: Inconsistència DICOM: La imatge " + dicomReader.getAttributeByName(DCM_SOPInstanceUID ) + " de la serie " + series->getInstanceUID() + " té el camp ImageType que és tipus 1, amb un nombre incorrecte d'elements: Valor del camp:: [" + value + "]" );
            }
        }
    }
    else
    {
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
