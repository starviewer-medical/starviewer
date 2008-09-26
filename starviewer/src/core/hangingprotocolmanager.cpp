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
#include "volume.h"
#include "q2dviewerwidget.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "identifier.h"
#include "logging.h"

#include <QList>

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject( parent )
{
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
					
					applyDisplayTransformations( patient, viewerWidget, displaySet );

                    DEBUG_LOG( tr("Image set number : %1, serie: %2, pos: %3").arg( imageSetNumber).arg(serie->getDescription()).arg(displaySet->getPosition()) );
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
    int numberSeries;
    QList< Study * > listOfStudies = patient->getStudies();
    QList< Series * > listOfSeries;
    Study * study;
    Series * serie;

    while (!found && i < numberStudies )
    {
        study = listOfStudies.value( i );
        numberSeries = study->getNumberOfSeries();
        listOfSeries = study->getSeries();
        j = 0;

        while( !found && j < numberSeries )
        {
            serie = listOfSeries.value( j );

            if( isValidSerie( serie, imageSet ) )
            {
                found = true;
                return serie;
            }
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

void HangingProtocolManager::applyDisplayTransformations( Patient * patient, Q2DViewerWidget * viewer, HangingProtocolDisplaySet * displaySet )
{
	QString patientDisplayOrientation = displaySet->getPatientOrientation();
	QString patientOrientation = viewer->getViewer()->getInput()->getSeries()->getPatientPosition();

}

}
