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

bool TemporalDimensionFillerStep::fillIndividually()
{
    SeriesInfo *seriesInfo;

    if ( TemporalDimensionInternalInfo.contains( m_input->getCurrentSeries() ))
    {
        seriesInfo = TemporalDimensionInternalInfo.value( m_input->getCurrentSeries() );
        seriesInfo->numberOfImages++;
        if ( !seriesInfo->isCTLocalizer )
        {
            if ( seriesInfo->firstImagePosition == m_input->getDICOMFile()->getAttributeByName( DCM_ImagePositionPatient ) )
            {
                seriesInfo->numberOfPhases++;
            }
        }
    }
    else
    {
        seriesInfo = new SeriesInfo;
        TemporalDimensionInternalInfo.insert( m_input->getCurrentSeries() , seriesInfo );

        seriesInfo->numberOfPhases = 1;
        seriesInfo->numberOfImages = 1;
        seriesInfo->isCTLocalizer = false;

        // en el cas del CT ens interessa saber si és localizer
        if( m_input->getCurrentSeries()->getModality() == "CT" )
        {
            QString value = m_input->getDICOMFile()->getAttributeByName( DCM_ImageType );
            QStringList valueList = value.split( "\\" );
            if( valueList.count() >= 3 )
            {
                if( valueList.at(2) == "LOCALIZER" )
                {
                    DEBUG_LOG("La serie amb uid " + m_input->getCurrentSeries()->getInstanceUID() + " no és dinàmica (És un CT LOCALIZER)" );
                    seriesInfo->isCTLocalizer = true;
                }
            }
            else
            {
                // TODO aquesta comprovació s'ha afegit perquè hem trobat un cas en que aquestes dades apareixen incoherents
                // tot i així, lo seu seria disposar d'alguna eina que comprovés si les dades són consistents o no.
                DEBUG_LOG( "ERROR: Inconsistència DICOM: La imatge " + m_input->getCurrentImage()->getSOPInstanceUID() + " de la serie " + m_input->getCurrentSeries()->getInstanceUID() + " té el camp ImageType que és tipus 1, amb un nombre incorrecte d'elements: Valor del camp:: [" + value + "]" );
            }
        }

        if ( ! seriesInfo->isCTLocalizer )
        {
            seriesInfo->firstImagePosition = m_input->getDICOMFile()->getAttributeByName( DCM_ImagePositionPatient );
        }
    }

    m_input->addLabelToSeries("TemporalDimensionFillerStep", m_input->getCurrentSeries() );

    return true;
}

void TemporalDimensionFillerStep::postProcessing()
{
    SeriesInfo *seriesInfo;
    foreach ( Series * key , TemporalDimensionInternalInfo.keys() )
    {
        seriesInfo = TemporalDimensionInternalInfo.take(key);
        key->setNumberOfPhases( seriesInfo->numberOfPhases );
        key->setNumberOfSlicesPerPhase( seriesInfo->numberOfImages / seriesInfo->numberOfPhases );
        if ( seriesInfo->numberOfPhases > 1 )
        {
            DEBUG_LOG("La sèrie " + key->getInstanceUID() + " és dinàmica");
        }

        delete seriesInfo;
    }
}

void TemporalDimensionFillerStep::abort()
{
    TemporalDimensionInternalInfo.clear();
}
        
void TemporalDimensionFillerStep::processSeries( Series *series )
{
    bool found = false;
    int phases = 1;
    int slices = 0;
    bool localizer = false;

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
                    localizer = true;
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
    if ( !localizer )
    {
        QString imagePositionPatient = dicomReader.getAttributeByName( DCM_ImagePositionPatient );

        if( !imagePositionPatient.isEmpty() )
        {
            while ( !found && phases < list.count() )
            {
                dicomReader.setFile( list[phases] );
                if ( imagePositionPatient == dicomReader.getAttributeByName( DCM_ImagePositionPatient  ) )
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
