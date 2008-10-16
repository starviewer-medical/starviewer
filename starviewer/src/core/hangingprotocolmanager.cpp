/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolmanager.h"

#include "viewerslayout.h"
#include "patient.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "q2dviewerwidget.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "dicomtagreader.h"
#include "identifier.h"
#include "logging.h"

#include <QList>

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject( parent )
{

	// Mappeig de les operacions per la vista sagital
	m_operationsMap.insert("A\\F-A\\H", "0,1"); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
	m_operationsMap.insert("A\\F-P\\F", "2,1"); m_operationsMap.insert("A\\F-P\\H", "2,0"); m_operationsMap.insert("A\\F-I\\A", "3,0");
	m_operationsMap.insert("A\\F-F\\A", "3,1"); m_operationsMap.insert("A\\F-F\\P", "1,0"); m_operationsMap.insert("A\\F-F\\P", "3,1");
	
	m_operationsMap.insert("A\\H-A\\F", "0,1"); m_operationsMap.insert("A\\H-P\\F", "2,0"); m_operationsMap.insert("A\\H-P\\H", "2,1");
	m_operationsMap.insert("A\\H-F\\A", "1,0"); m_operationsMap.insert("A\\H-H\\A", "3,1"); m_operationsMap.insert("A\\H-F\\P", "1,1");
	m_operationsMap.insert("A\\H-H\\P", "3,0");
	
	m_operationsMap.insert("P\\F-A\\F", "2,1"); m_operationsMap.insert("P\\F-A\\H", "2,0"); m_operationsMap.insert("P\\F-P\\H", "0,1");
	m_operationsMap.insert("P\\F-F\\A", "3,0"); m_operationsMap.insert("P\\F-H\\A", "1,1"); m_operationsMap.insert("P\\F-F\\P", "3,1");
	m_operationsMap.insert("P\\F-H\\P", "1,0");
	
	m_operationsMap.insert("P\\H-A\\F", "2,0"); m_operationsMap.insert("P\\H-A\\H", "2,1"); m_operationsMap.insert("P\\H-P\\F", "0,1");
	m_operationsMap.insert("P\\H-F\\A", "1,1"); m_operationsMap.insert("P\\H-H\\A", "3,0"); m_operationsMap.insert("P\\H-F\\P", "1,0");
	m_operationsMap.insert("P\\H-H\\P", "3,1");
	

	m_operationsMap.insert("F\\A-A\\F", "3,1"); m_operationsMap.insert("F\\A-A\\H", "3,0"); m_operationsMap.insert("F\\A-P\\F", "1,0"); 
	m_operationsMap.insert("F\\A-P\\H", "1,1"); m_operationsMap.insert("F\\A-H\\A", "2,1"); m_operationsMap.insert("F\\A-F\\P", "0,1");
	m_operationsMap.insert("F\\A-H\\P", "2,0");

	m_operationsMap.insert("H\\A-A\\F", "3,0"); m_operationsMap.insert("H\\A-A\\H", "3,1"); m_operationsMap.insert("H\\A-P\\F", "1,1");
	m_operationsMap.insert("H\\A-P\\H", "1,0"); m_operationsMap.insert("H\\A-F\\A", "2,1"); m_operationsMap.insert("H\\A-F\\P", "2,0");
	m_operationsMap.insert("H\\A-H\\P", "0,1");

	m_operationsMap.insert("F\\P-A\\F", "1,0"); m_operationsMap.insert("F\\P-A\\H", "1,1"); m_operationsMap.insert("F\\P-P\\F", "3,1");
	m_operationsMap.insert("F\\P-P\\H", "3,0"); m_operationsMap.insert("F\\P-F\\A", "0,1"); m_operationsMap.insert("F\\P-H\\A", "2,0");
	m_operationsMap.insert("F\\P-H\\P", "2,1");

	m_operationsMap.insert("S\\P-A\\F", "3,1"); m_operationsMap.insert("S\\P-A\\H", "1,0"); m_operationsMap.insert("S\\P-P\\F", "3,0");
	m_operationsMap.insert("S\\P-P\\S", "1,1"); m_operationsMap.insert("S\\P-F\\A", "2,0"); m_operationsMap.insert("S\\P-H\\A", "0,1");
	m_operationsMap.insert("S\\P-F\\P", "2,1");

}


HangingProtocolManager::~HangingProtocolManager()
{
}

void HangingProtocolManager::searchAndApplyBestHangingProtocol( ViewersLayout * layout, Patient * patient)
{
    Identifier id;
    HangingProtocol * hangingPotocol;
	HangingProtocol * bestHangingProtocol = NULL;
	QList<Series *> selectedSeries;
	QList<Series *> bestSelectedSeries;
	double adjustmentOfHanging = 0.0; // Inicialment pensem que no existeix cap hanging
	double bestAdjustmentOfHanging = 0.0; // Inicialment pensem que no existeix cap hanging
	int numberOfItems = HangingProtocolsRepository::getRepository()->getNumberOfItems();
    int hangingProtocolNumber;
    int imageSetNumber;
	int displaySetNumber;
    HangingProtocolImageSet * imageSet;
    HangingProtocolDisplaySet *displaySet;
    Series * serie;
	Q2DViewerWidget * viewerWidget;

	int numberOfSeriesAssigned;

	// Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
	// Aprofitem per assignar ja les series, per millorar el rendiment
	for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfItems; hangingProtocolNumber++)
	{	
		id.setValue( hangingProtocolNumber );
        hangingPotocol = HangingProtocolsRepository::getRepository()->getItem( id );
		selectedSeries.clear();
		numberOfSeriesAssigned = 0;
		imageSetNumber = 1;
		serie = NULL;

		if( isValid( hangingPotocol, patient ) )
        {
			while( imageSetNumber <= hangingPotocol->getNumberOfImageSets() )
            {
				DEBUG_LOG( tr("Agafo image SET") );

				imageSet = hangingPotocol->getImageSet( imageSetNumber );
                serie = searchSerie( patient, imageSet );

                if( serie)
				{
					DEBUG_LOG( tr("TINC SERIE!") );
					selectedSeries << serie;
					numberOfSeriesAssigned++;
				}
				imageSetNumber++;
			}
			adjustmentOfHanging = ((double)numberOfSeriesAssigned)/hangingPotocol->getNumberOfImageSets();
			
			DEBUG_LOG( tr("ADJUSTMENT: %1").arg( adjustmentOfHanging ) );

			if( adjustmentOfHanging > bestAdjustmentOfHanging )
			{
				bestHangingProtocol = hangingPotocol;
				bestSelectedSeries.clear();
				bestSelectedSeries << selectedSeries;
				bestAdjustmentOfHanging = adjustmentOfHanging;

				if( bestAdjustmentOfHanging == 1.0 )/// No en trobarem cap de millor
					break;
			}
		}
	}

	// Aplicar el hanging protocol trobat, si és que se n'ha trobat algun
	if( bestHangingProtocol )
	{
		for( displaySetNumber = 0; displaySetNumber < bestHangingProtocol->getNumberOfDisplaySets(); displaySetNumber ++)
		{
			serie = NULL;
			//imageSet = hangingPotocol->getImageSet( imageSetNumber + 1 ); // Els id's comencen a 1
			displaySet = hangingPotocol->getDisplaySet( displaySetNumber + 1 );
			imageSet = hangingPotocol->getImageSet( displaySet->getImageSetNumber() );
			serie = imageSet->getSeriesToDisplay();
			//displaySet = hangingPotocol->getDisplaySetOfImageSet( imageSet->getIdentifier() );
			viewerWidget = layout->addViewer( displaySet->getPosition() );
			
			if( serie ) // Ens podem trobar que un viewer no tingui serie, llavors no hi posem input
			{			
				viewerWidget->setInput( serie->getFirstVolume() );

				if( imageSet->getTypeOfItem() == "image" )
				{
					viewerWidget->getViewer()->setSlice( imageSet->getImatgeToDisplay() );
					applyDisplayTransformations( patient, serie, imageSet->getImatgeToDisplay(), viewerWidget, displaySet);
				}
				else
				{
					applyDisplayTransformations( patient, serie, 0, viewerWidget, displaySet);
				}
			}
		}
	}
}

bool HangingProtocolManager::isValid( HangingProtocol * protocol, Patient * patient)
{
    bool valid = false;
    bool found = false;
    int i;

    foreach (Study *study, patient->getStudies())
    {
        QList<QString> listOfModalities = study->getModalitiesAsSingleString().split("/");
        found = false;
        i = 0;

        while( !found  && i < listOfModalities.size() )
        {
            if( protocol->getHangingProtocolMask()->getProtocolList().contains( listOfModalities.value(i) ) )
            {
                valid = true;
                found = true;
            }
            i++;
        }
    }
    return valid;
}

Series * HangingProtocolManager::searchSerie( Patient * patient, HangingProtocolImageSet * imageSet )
{
    bool found = false;
    int numberStudies = patient->getNumberOfStudies();
    int i = 0;
    int j = 0;
	int imageNumber = 0;
    int numberSeries;
	int numberImages;
    QList< Study * > listOfStudies = patient->getStudies();
    QList< Series * > listOfSeries;
	QList< Image * > listOfImages;
    Study * study;
    Series * serie = NULL;
	Image * image;

    while (!found && i < numberStudies )
    {
        study = listOfStudies.value( i );
        numberSeries = study->getNumberOfSeries();
        listOfSeries = study->getSeries();
        j = 0;

        while( !found && j < numberSeries )
        {
            serie = listOfSeries.value( j );

			if( imageSet->getTypeOfItem() != "image" )
			{
				if( isValidSerie( serie, imageSet ) )
				{
					found = true;
				}
			}
			else
			{
				imageNumber = 0;
				listOfImages = serie->getImages();
				numberImages = listOfImages.size();

				while( !found && imageNumber < numberImages )
				{
					image = listOfImages.value( imageNumber );
					if( isValidImage( image, imageSet ) )
					{
						found = true;
						imageSet->setImageToDisplay( imageNumber );
						imageSet->setSeriesToDisplay( serie );
					}
					imageNumber++;
				}
				if( !found ) 
					imageSet->setImageToDisplay( 0 );
			}

			if( found ) 
				return serie;
            j++;
        }
        i++;
    }

    return NULL;
}

bool HangingProtocolManager::isValidSerie( Series * serie, HangingProtocolImageSet * imageSet )
{
    bool valid = true;
    int i = 0;
    QList< HangingProtocolImageSet::Restriction > listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;

    while ( valid && i < numberRestrictions )
    {
        restriction = listOfRestrictions.value( i );

        if( restriction.selectorAttribute == "StudyDescription" )
        {

        }
        else if( restriction.selectorAttribute == "BodyPartExamined" )
        {
            if( serie->getBodyPartExamined() != restriction.valueRepresentation ) valid = false;
        }
        else if( restriction.selectorAttribute == "ProtocolName" )
        {
            if( serie->getProtocolName() != restriction.valueRepresentation ) valid = false;
        }
        else if( restriction.selectorAttribute == "ViewPosition" )
        {
            if( serie->getViewPosition() != restriction.valueRepresentation ) valid = false;
        }
        else if( restriction.selectorAttribute == "Laterality" )
        {

        }
        else if( restriction.selectorAttribute == "SeriesDescription" )
        {
            if( serie->getDescription() != restriction.valueRepresentation ) valid = false;
        }
		else if( restriction.selectorAttribute == "PatientOrientation" )
		{
		
		}
        i++;
    }

    return valid;
}

void HangingProtocolManager::applyDisplayTransformations( Patient * patient, Series * serie, int imageNumber, Q2DViewerWidget * viewer, HangingProtocolDisplaySet * displaySet )
{
	QString rowsImage;
	QString columnsImage;
	QString rowsDisplay;
	QString columnsDisplay;
	QList<QString> listOfDirections;

	QString patientDisplayOrientation = displaySet->getPatientOrientation();
	/*QString patientOrientation = (serie->getImages()[0])->getPatientOrientation();

	listOfDirections = patientOrientation.split(",");
	if( listOfDirections.size() == 2 )
	{
		rowsImage = listOfDirections[0];
		columnsImage = listOfDirections[1];
	}

	listOfDirections = patientDisplayOrientation.split("/");
	if( listOfDirections.size() == 2 )
	{
		rowsDisplay = listOfDirections[0];
		columnsDisplay = listOfDirections[1];
	}
	
	if( (rowsImage != "") && (rowsDisplay != "") && ( (rowsImage == "P" && rowsDisplay == "A") || (rowsImage == "A" && rowsDisplay == "P") ) )
	{
		viewer->getViewer()->horizontalFlip();
	}*/


	DICOMTagReader dicomReader;
	QString patientOrientation = 0;
	int i;
	QList<QString> listOfOperations;
	QString operations = 0;
	int rotations;
	int flips;

	bool ok = dicomReader.setFile( serie->getImages()[imageNumber]->getPath() );
	if( ok && (patientDisplayOrientation != "" ) )
	{
		patientOrientation = dicomReader.getAttributeByName( DCM_PatientOrientation );
		if( patientOrientation != 0 )
		{
			patientOrientation.truncate(3);
			patientOrientation.append("-").append(patientDisplayOrientation);
			QString mapIndex( patientOrientation );
			operations = m_operationsMap.value( mapIndex );

			DEBUG_LOG( tr("\n\nVALOR A BUSCAR AL MAP: %1\n\n").arg( mapIndex) );

			if( operations != 0 )
			{
				listOfOperations = operations.split(",");
				rotations = listOfOperations[0].toInt();
				flips = listOfOperations[1].toInt();

				for( i = 0; i < rotations; i++ ) // Totes les rotacions a realitzar
				{
					viewer->getViewer()->rotateClockWise();
				}

				for( i = 0; i < flips; i++ ) // Totes les rotacions a realitzar
				{
					viewer->getViewer()->verticalFlip();
				}
			}
		}
	}
}

bool HangingProtocolManager::isValidImage( Image * image, HangingProtocolImageSet * imageSet )
{
	bool valid = true;
    int i = 0;
    QList< HangingProtocolImageSet::Restriction > listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;
	DICOMTagReader dicomReader;

	bool ok = dicomReader.setFile( image->getPath() );
	if( ok )
	{
		while ( valid && i < numberRestrictions )
		{
			restriction = listOfRestrictions.value( i );
		
			if( restriction.selectorAttribute == "ViewPosition" )
			{
				if( dicomReader.getAttributeByName( DCM_ViewPosition ) != restriction.valueRepresentation )
					valid = false;
			}
			else if( restriction.selectorAttribute == "Laterality" )
			{
				if( dicomReader.getAttributeByName( DCM_ImageLaterality ) != restriction.valueRepresentation )
					valid = false;
			}
			else if( restriction.selectorAttribute == "PatientOrientation" )
			{
				if( ! dicomReader.getAttributeByName( DCM_PatientOrientation ).contains( restriction.valueRepresentation ) )
					valid = false;
			}
			i++;
		}
	}
    return valid;
}

}