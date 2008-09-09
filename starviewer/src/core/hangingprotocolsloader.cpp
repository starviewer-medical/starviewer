/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolsloader.h"

#include "logging.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolxmlreader.h"
#include "identifier.h"

// Qt's
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
namespace udg {

HangingProtocolsLoader::HangingProtocolsLoader(QObject *parent)
 : QObject( parent )
{

}


HangingProtocolsLoader::~HangingProtocolsLoader()
{

}

void HangingProtocolsLoader::loadDefaults()
{
    QSettings systemSettings;
    QString path = 0;
    path = systemSettings.value("Hanging-Protocols/path").toString();
    if( path != 0 )
        loadXMLFile( path );
}

void HangingProtocolsLoader::loadMamoGuell()
{
    /// Protocol només per fer un layout de 2x2 sense tenir en compte ni l'usuari ni res més...
    HangingProtocol * protocol2x2 = new HangingProtocol();
    protocol2x2->setNumberOfScreens( 1 );
    QList<QString> protocolsList;
    protocolsList << "CR";
    protocol2x2->setProtocolsList( protocolsList );

    /// Restriccions
    HangingProtocolImageSet::Restriction restriction_1;
    restriction_1.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_1.selectorAttribute = "StudyDescription";
    restriction_1.valueRepresentation = "MAMOGRAFIA";

    HangingProtocolImageSet::Restriction restriction_2;
    restriction_2.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_2.selectorAttribute = "BodyPartExamined";
    restriction_2.valueRepresentation = "MAMOGRAFIA";

    HangingProtocolImageSet::Restriction restriction_4;
    restriction_4.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_4.selectorAttribute = "ViewPosition";
    restriction_4.valueRepresentation = "CCD";

    HangingProtocolImageSet::Restriction restriction_5;
    restriction_5.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_5.selectorAttribute = "ViewPosition";
    restriction_5.valueRepresentation = "CCI";

    HangingProtocolImageSet::Restriction restriction_6;
    restriction_6.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_6.selectorAttribute = "ViewPosition";
    restriction_6.valueRepresentation = "OBLD";

    HangingProtocolImageSet::Restriction restriction_7;
    restriction_7.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_7.selectorAttribute = "ViewPosition";
    restriction_7.valueRepresentation = "OBLI";

    HangingProtocolImageSet::Restriction restriction_8;
    restriction_8.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_8.selectorAttribute = "Laterality";
    restriction_8.valueRepresentation = "L";

    HangingProtocolImageSet::Restriction restriction_9;
    restriction_9.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_9.selectorAttribute = "Laterality";
    restriction_9.valueRepresentation = "R";

    /// Image sets
    HangingProtocolImageSet * imageSet_1 = new HangingProtocolImageSet();
    imageSet_1->setIdentifier( 1 );
    imageSet_1->addRestriction( restriction_1 );
    imageSet_1->addRestriction( restriction_2 );
    imageSet_1->addRestriction( restriction_6 );
    imageSet_1->addRestriction( restriction_9 );

    HangingProtocolImageSet * imageSet_2 = new HangingProtocolImageSet();
    imageSet_2->setIdentifier( 2 );
    imageSet_2->addRestriction( restriction_1 );
    imageSet_2->addRestriction( restriction_2 );
    imageSet_2->addRestriction( restriction_7 );
    imageSet_2->addRestriction( restriction_8 );

    HangingProtocolImageSet * imageSet_3 = new HangingProtocolImageSet();
    imageSet_3->setIdentifier( 3 );
    imageSet_3->addRestriction( restriction_1 );
    imageSet_3->addRestriction( restriction_2 );
    imageSet_3->addRestriction( restriction_4 );
    imageSet_3->addRestriction( restriction_9 );


    HangingProtocolImageSet * imageSet_4 = new HangingProtocolImageSet();
    imageSet_4->setIdentifier( 4 );
    imageSet_4->addRestriction( restriction_1 );
    imageSet_4->addRestriction( restriction_2 );
    imageSet_4->addRestriction( restriction_5 );
    imageSet_4->addRestriction( restriction_8 );

    protocol2x2->addImageSet( imageSet_1 );
    protocol2x2->addImageSet( imageSet_2 );
    protocol2x2->addImageSet( imageSet_3 );
    protocol2x2->addImageSet( imageSet_4 );

    /// Display sets
    HangingProtocolDisplaySet * displaySet_1 = new HangingProtocolDisplaySet();
    displaySet_1->setIdentifier( 1 );
    displaySet_1->setImageSetNumber( 1 );
    displaySet_1->setPosition( "0.0\\1.0\\0.5\\0.5" );

    HangingProtocolDisplaySet * displaySet_2 = new HangingProtocolDisplaySet();
    displaySet_2->setIdentifier( 2 );
    displaySet_2->setImageSetNumber( 2 );
    displaySet_2->setPosition( "0.5\\ 1.0\\1.0\\0.5" );

    HangingProtocolDisplaySet * displaySet_3 = new HangingProtocolDisplaySet();
    displaySet_3->setIdentifier( 3 );
    displaySet_3->setImageSetNumber( 3 );
    displaySet_3->setPosition( "0.0\\0.5\\0.5\\0.0" );

    HangingProtocolDisplaySet * displaySet_4 = new HangingProtocolDisplaySet();
    displaySet_4->setIdentifier( 4 );
    displaySet_4->setImageSetNumber( 4 );
    displaySet_4->setPosition( "0.5\\0.5\\1.0\\0.0" );

    protocol2x2->addDisplaySet( displaySet_1 );
    protocol2x2->addDisplaySet( displaySet_2 );
    protocol2x2->addDisplaySet( displaySet_3 );
    protocol2x2->addDisplaySet( displaySet_4 );

    HangingProtocolsRepository::getRepository()->addItem( protocol2x2 );
}

void HangingProtocolsLoader::loadMamoTrueta()
{
    /// Protocol només per fer un layout de 2x2 sense tenir en compte ni l'usuari ni res més...
    HangingProtocol * protocol2x2 = new HangingProtocol();
    protocol2x2->setNumberOfScreens( 1 );
    QList<QString> protocolsList;
    protocolsList << "MG";
    protocol2x2->setProtocolsList( protocolsList );

    /// Restriccions
    HangingProtocolImageSet::Restriction restriction_1;
    restriction_1.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_1.selectorAttribute = "SeriesDescription";
    restriction_1.valueRepresentation = "R CC";

    HangingProtocolImageSet::Restriction restriction_2;
    restriction_2.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_2.selectorAttribute = "SeriesDescription";
    restriction_2.valueRepresentation = "R MLO";

    HangingProtocolImageSet::Restriction restriction_3;
    restriction_3.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_3.selectorAttribute = "SeriesDescription";
    restriction_3.valueRepresentation = "L CC";

    HangingProtocolImageSet::Restriction restriction_4;
    restriction_4.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_4.selectorAttribute = "SeriesDescription";
    restriction_4.valueRepresentation = "L MLO";

    HangingProtocolImageSet::Restriction restriction_5;
    restriction_5.usageFlag = HangingProtocolImageSet::MATCH;
    restriction_5.selectorAttribute = "BodyPartExamined";
    restriction_5.valueRepresentation = "BREAST";

    /// Image sets
    HangingProtocolImageSet * imageSet_1 = new HangingProtocolImageSet();
    imageSet_1->setIdentifier( 1 );
    imageSet_1->addRestriction( restriction_5 );
    imageSet_1->addRestriction( restriction_2 );

    HangingProtocolImageSet * imageSet_2 = new HangingProtocolImageSet();
    imageSet_2->setIdentifier( 2 );
    imageSet_2->addRestriction( restriction_5 );
    imageSet_2->addRestriction( restriction_4 );

    HangingProtocolImageSet * imageSet_3 = new HangingProtocolImageSet();
    imageSet_3->setIdentifier( 3 );
    imageSet_3->addRestriction( restriction_5 );
    imageSet_3->addRestriction( restriction_1 );

    HangingProtocolImageSet * imageSet_4 = new HangingProtocolImageSet();
    imageSet_4->setIdentifier( 4 );
    imageSet_4->addRestriction( restriction_5 );
    imageSet_4->addRestriction( restriction_3 );

    protocol2x2->addImageSet( imageSet_1 );
    protocol2x2->addImageSet( imageSet_2 );
    protocol2x2->addImageSet( imageSet_3 );
    protocol2x2->addImageSet( imageSet_4 );

    /// Display sets
    HangingProtocolDisplaySet * displaySet_1 = new HangingProtocolDisplaySet();
    displaySet_1->setIdentifier( 1 );
    displaySet_1->setImageSetNumber( 1 );
    displaySet_1->setPosition( "0.0\\1.0\\0.5\\0.5" );

    HangingProtocolDisplaySet * displaySet_2 = new HangingProtocolDisplaySet();
    displaySet_2->setIdentifier( 2 );
    displaySet_2->setImageSetNumber( 2 );
    displaySet_2->setPosition( "0.5\\ 1.0\\1.0\\0.5" );

    HangingProtocolDisplaySet * displaySet_3 = new HangingProtocolDisplaySet();
    displaySet_3->setIdentifier( 3 );
    displaySet_3->setImageSetNumber( 3 );
    displaySet_3->setPosition( "0.0\\0.5\\0.5\\0.0" );

    HangingProtocolDisplaySet * displaySet_4 = new HangingProtocolDisplaySet();
    displaySet_4->setIdentifier( 4 );
    displaySet_4->setImageSetNumber( 4 );
    displaySet_4->setPosition( "0.5\\0.5\\1.0\\0.0" );

    protocol2x2->addDisplaySet( displaySet_1 );
    protocol2x2->addDisplaySet( displaySet_2 );
    protocol2x2->addDisplaySet( displaySet_3 );
    protocol2x2->addDisplaySet( displaySet_4 );

    HangingProtocolsRepository::getRepository()->addItem( protocol2x2 );
}

bool HangingProtocolsLoader::loadXMLFile( QString filePath )
{
    HangingProtocolXMLReader * xmlReader = new HangingProtocolXMLReader();
    QList<HangingProtocol * > listHangingProtocols = xmlReader->readFile( filePath );

    if( listHangingProtocols.size() > 0 )
    {
        foreach( HangingProtocol * hangingProtocol, listHangingProtocols )
        {
            HangingProtocolsRepository::getRepository()->addItem( hangingProtocol );
            hangingProtocol->show();
        }
    }

    return true;
}
}
