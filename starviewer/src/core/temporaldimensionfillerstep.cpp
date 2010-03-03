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
#include "dicomdictionary.h"
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
    VolumeInfo * volumeInfo;
    QHash< int , VolumeInfo * > * volumeHash;

    foreach ( Series * key , TemporalDimensionInternalInfo.keys() )
    {
        volumeHash = TemporalDimensionInternalInfo.take(key);

        foreach ( int volumeNumber, volumeHash->keys() )
        {
            volumeInfo = volumeHash->take( volumeNumber );
            delete volumeInfo;
        }
        
        delete volumeHash;
    }
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
    VolumeInfo *volumeInfo;
    bool volumeInfoInitialized = false;
    
    //Obtenim el VolumeInfo. Si no existeix en generem un de nou i l'afegim a l'estructura.
    if ( TemporalDimensionInternalInfo.contains( m_input->getCurrentSeries() ) )
    {
        QHash< int , VolumeInfo * > *volumeHash = TemporalDimensionInternalInfo.value( m_input->getCurrentSeries() );
       
        if ( volumeHash->contains( m_input->getCurrentVolumeNumber() ) )
        {
            volumeInfoInitialized = true;
            volumeInfo = volumeHash->value( m_input->getCurrentVolumeNumber() );
        }
        else
        {
            volumeInfo = new VolumeInfo;        
            volumeHash->insert( m_input->getCurrentVolumeNumber() , volumeInfo );
        }
    }
    else
    {
        QHash< int , VolumeInfo * > * volumeHash = new QHash< int , VolumeInfo * >();
        volumeInfo = new VolumeInfo;
        volumeHash->insert( m_input->getCurrentVolumeNumber() , volumeInfo );
        TemporalDimensionInternalInfo.insert( m_input->getCurrentSeries() , volumeHash );
    }
    
    //Si el VolumeInfo és nou, l'inicialitzem.
    if ( !volumeInfoInitialized )
    {
        volumeInfo->numberOfPhases = 1;
        volumeInfo->numberOfImages = 0;
        volumeInfo->isCTLocalizer = false;
        volumeInfo->firstImagePosition = "";

        // en el cas del CT ens interessa saber si és localizer
        // \TODO Ara estem considerant que un volume serà localizer si la primera imatge ho és, però res ens indica que els localizers no puguin està barretjats amb la resta.
        if( m_input->getCurrentSeries()->getModality() == "CT" )
        {
            QString value = m_input->getCurrentImages().at(0)->getImageType();
            QStringList valueList = value.split( "\\" );
            if( valueList.count() >= 3 )
            {
                if( valueList.at(2) == "LOCALIZER" )
                {
                    DEBUG_LOG("La serie amb uid " + m_input->getCurrentSeries()->getInstanceUID() + " no és dinàmica (És un CT LOCALIZER)" );
                    volumeInfo->isCTLocalizer = true;
                }
            }
            else
            {
                // TODO aquesta comprovació s'ha afegit perquè hem trobat un cas en que aquestes dades apareixen incoherents
                // tot i així, lo seu seria disposar d'alguna eina que comprovés si les dades són consistents o no.
                DEBUG_LOG( "ERROR: Inconsistència DICOM: La imatge " + m_input->getCurrentImage()->getSOPInstanceUID() + " de la serie " + m_input->getCurrentSeries()->getInstanceUID() + " té el camp ImageType que és tipus 1, amb un nombre incorrecte d'elements: Valor del camp:: [" + value + "]" );
            }
        }
    }
    
    // Si és CTLocalizer no cal recorre totes les imatges ja que només ens interessa saber quantes n'hem d'afegir al VolumeInfo.
    if ( volumeInfo->isCTLocalizer )
    {
        volumeInfo->numberOfImages += m_input->getCurrentImages().count();
    }
    else
    {
        foreach ( Image * image, m_input->getCurrentImages() )
        {
            const double * imagePositionPatient = image->getImagePositionPatient();
            
            if( !(imagePositionPatient[0] == 0. &&  imagePositionPatient[1] == 0. && imagePositionPatient[2] == 0.) )
	        {
		        QString imagePositionPatientString = QString("%1\\%2\\%3").arg(imagePositionPatient[0])
                                                                          .arg(imagePositionPatient[1])
                                                                          .arg(imagePositionPatient[2]);

                if ( volumeInfo->firstImagePosition.isEmpty() )
                {
                    volumeInfo->firstImagePosition = imagePositionPatientString;
                }
                else
                {
                    if ( volumeInfo->firstImagePosition == imagePositionPatientString )
		            {
			            volumeInfo->numberOfPhases++;
		            }
                }
	        }

            volumeInfo->numberOfImages++;
        }
    }

    
    m_input->addLabelToSeries("TemporalDimensionFillerStep", m_input->getCurrentSeries() );

    return true;
}

void TemporalDimensionFillerStep::postProcessing()
{
    int currentVolume = -1;
    int currentPhase;
    int numberOfPhases;

    foreach ( Series * key , TemporalDimensionInternalInfo.keys() )
    {
        QHash< int , VolumeInfo * > *volumeHash = TemporalDimensionInternalInfo.take( key );
       
        foreach( Image *image, key->getImages() )
        {
            if ( currentVolume != image->getVolumeNumberInSeries() )
            {
                currentVolume = image->getVolumeNumberInSeries();
                if ( volumeHash->contains( currentVolume ) )
                {
                    VolumeInfo * volumeInfo = volumeHash->take( currentVolume );
                
                    numberOfPhases = volumeInfo->numberOfPhases;
                    // L'esborrem perquè ja no el necessitarem més
                    if ( volumeInfo )
                        delete volumeInfo;
                }
                else
                {
                    numberOfPhases = 1;

                    ERROR_LOG(QString("El volume %1 de la sèrie %2 no ha estat processat! Considerem que no és dinàmic").arg(currentVolume).arg(key->getInstanceUID()));
                    DEBUG_LOG(QString("El volume %1 de la sèrie %2 no ha estat processat! Considerem que no és dinàmic").arg(currentVolume).arg(key->getInstanceUID()));
                }

                currentPhase = 0;

                if ( numberOfPhases > 1 )
                {
                    DEBUG_LOG(QString("El volume %1 de la serie %2 és dinamic").arg(currentVolume).arg(key->getInstanceUID()) );
                }

            }

            image->setPhaseNumber( currentPhase );

            currentPhase++;
            if ( currentPhase == numberOfPhases )
                currentPhase = 0;
        }

        currentVolume = -1;
        
        delete volumeHash;
    }
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
    if (dicomReader.tagExists( DICOMImageType ))
    {
        // aquest valor és de tipus 3 al mòdul General Image, però consta com a tipus 1 a
        // gairebé totes les modalitats. Només consta com a tipus 2 per la modalitat US
        QString value = dicomReader.getAttributeByName( DICOMImageType );
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
                DEBUG_LOG( "ERROR: Inconsistència DICOM: La imatge " + dicomReader.getAttributeByName(DICOMSOPInstanceUID ) + " de la serie " + series->getInstanceUID() + " té el camp ImageType que és tipus 1, amb un nombre incorrecte d'elements: Valor del camp:: [" + value + "]" );
            }
        }
    }
    if ( !localizer )
    {
        QString imagePositionPatient = dicomReader.getAttributeByName( DICOMImagePositionPatient );

        if( !imagePositionPatient.isEmpty() )
        {
            while ( !found && phases < list.count() )
            {
                dicomReader.setFile( list[phases] );
                if ( imagePositionPatient == dicomReader.getAttributeByName( DICOMImagePositionPatient  ) )
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
