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
	//m_operationsMap.insert("AFAS", 0, 1); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
	//m_operationsMap.insert("AFPI", 2, 1); m_operationsMap.insert("AFPS", 2, 0); m_operationsMap.insert("AFIA", 3, 0);
	//m_operationsMap.insert("AFSA", 3, 1); m_operationsMap.insert("AFIP", 1, 0); m_operationsMap.insert("AFSP", 3, 1);
	
	//m_operationsMap.insert("AHAI", 0, 1); m_operationsMap.insert("AHPI", 2, 0); m_operationsMap.insert("AHPS", 2, 1);
	//m_operationsMap.insert("AHIA", 1, 0); m_operationsMap.insert("AHSA", 3, 1);	m_operationsMap.insert("AHIP", 1, 1);
	//m_operationsMap.insert("AHSP", 3, 0);
	//
	//m_operationsMap.insert("PFAI", 2, 1); m_operationsMap.insert("PFAS", 2, 0);	m_operationsMap.insert("PFPS", 0, 1);
	//m_operationsMap.insert("PFIA", 3, 0); m_operationsMap.insert("PFSA", 1, 1);	m_operationsMap.insert("PFIP", 3, 1);
	//m_operationsMap.insert("PFSP", 1, 0);
	//
	//m_operationsMap.insert("AFAS", 0, 1); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
	//m_operationsMap.insert("AFPI", 2, 1);
	//m_operationsMap.insert("AFPS", 2, 0);
	//m_operationsMap.insert("AFIA", 3, 0);
	//m_operationsMap.insert("AFSA", 3, 1);
	//m_operationsMap.insert("AFIP", 1, 0);
	//m_operationsMap.insert("AFSP", 3, 1);
	

	//m_operationsMap.insert("",,);
	//m_operationsMap.insert("",,);
	//m_operationsMap.insert("",,);

}


HangingProtocolManager::~HangingProtocolManager()
{
}

void HangingProtocolManager::searchAndApplyBestHangingProtocol( ViewersLayout * layout, Patient * patient)
{
    Identifier id;
    HangingProtocol * hangingPotocol;
    int numberOfItems = HangingProtocolsRepository::getRepository()->getNumberOfItems();
    int i;
    int imageSetNumber = 1;
    HangingProtocolImageSet * imageSet;
    HangingProtocolDisplaySet *displaySet;
    Series * serie;
    QList<QString> positions;
    QList<Series *> series;
	Q2DViewerWidget * viewerWidget;

    for( i = 0; i < numberOfItems; i++)
    {
        id.setValue( i );
        hangingPotocol = HangingProtocolsRepository::getRepository()->getItem( id );

        if( isValid( hangingPotocol, patient ) )
        {

            while( imageSetNumber <= hangingPotocol->getNumberOfImageSets() )
            {
                /// Busar la sèrie
                imageSet = hangingPotocol->getImageSet( imageSetNumber );
                serie = searchSerie( patient, imageSet );

                if( serie)
                {
                    displaySet = hangingPotocol->getDisplaySetOfImageSet( imageSet->getIdentifier() );

                    positions << displaySet->getPosition();
                    series << serie;

					viewerWidget = layout->addViewer( displaySet->getPosition() );
					viewerWidget->setInput( serie->getFirstVolume() );
					applyDisplayTransformations( patient, serie, viewerWidget, displaySet);

					if( imageSet->getTypeOfItem() == "image" )
					{
						viewerWidget->getViewer()->setSlice( imageSet->getImatgeToDisplay() );
					}
                }

                imageSetNumber++;
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
    Series * serie;
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

        i++;
    }

    return valid;
}

void HangingProtocolManager::applyDisplayTransformations( Patient * patient, Series * serie, Q2DViewerWidget * viewer, HangingProtocolDisplaySet * displaySet )
{
	QString rowsImage;
	QString columnsImage;
	QString rowsDisplay;
	QString columnsDisplay;
	QList<QString> listOfDirections;

	QString patientDisplayOrientation = displaySet->getPatientOrientation();
	QString patientOrientation = (serie->getImages()[0])->getPatientOrientation();

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
	}

}

bool HangingProtocolManager::isValidImage( Image * image, HangingProtocolImageSet * imageSet )
{
	int numberOfImage = -1;
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
		i++;
		}
	}
    return valid;
}

}