/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "hangingprotocolxmlreader.h"

#if QT_VERSION >= 0x040300

#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include "logging.h"
// Qt's
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QFileInfoList>


namespace udg {

HangingProtocolXMLReader::HangingProtocolXMLReader(QObject *parent)
 : QObject(parent)
{
}


HangingProtocolXMLReader::~HangingProtocolXMLReader()
{
}

QList<HangingProtocol * > HangingProtocolXMLReader::read( QString path )
{
	QFileInfo fileToRead( path );
	QFileInfo file;
	QList<HangingProtocol * > protocols;
	QList<HangingProtocol * > directoryProtocols;
	QFileInfoList entryInfoList;
	int numberOfFileInfo;
	int i;

	if( fileToRead.isDir() )
	{
		QDir directory( path );
		entryInfoList = directory.entryInfoList();
		numberOfFileInfo = entryInfoList.size();
		
		for( i = 0; i < numberOfFileInfo; i++)
		{
			file = entryInfoList.value( i );

			if( (file.fileName() != ".") && (file.fileName() != "..") )
			{
				directoryProtocols = read( file.absolutePath() + QDir::toNativeSeparators( "/" ) + file.fileName() ); 
				protocols << directoryProtocols;
			}
		}
	}
	else
	{
		if( fileToRead.suffix() == "xml" )
		{
			protocols = readFile( path );
		}
	}
	return protocols;
}

QList<HangingProtocol * > HangingProtocolXMLReader::readFile( QString path )
{
    QFile file(path);
    HangingProtocol * hangingProtocol = 0;
    QList< QString > protocols;
    HangingProtocolImageSet::Restriction restriction;
    HangingProtocolImageSet * imageSet;
    HangingProtocolDisplaySet * displaySet;
    QList<HangingProtocol * > listHangingProtocols;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("Hanging protocol XML File"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return listHangingProtocols;
    }

    QXmlStreamReader * reader = new QXmlStreamReader(&file);

    while (!reader->atEnd()) {

        reader->readNext();

        if( reader->tokenType() == QXmlStreamReader::StartElement )
        {
            if( reader->name() == "hangingProtocol")
            {
                hangingProtocol = new HangingProtocol();
                protocols.clear();
                m_restrictionsList.clear();

                while( !reader->atEnd() )
                {
                    reader->readNext();
                    reader->readNext();

                    if( reader->name() == "hangingProtocolName" )
                    {
                        reader->readNext();
                        hangingProtocol->setName( reader->text().toString() );
                        reader->readNext();
                    }
                    else if( reader->name() == "numberScreens" )
                    {
                        reader->readNext();
                        hangingProtocol->setNumberOfScreens( reader->text().toString().toInt() );
                        reader->readNext();
                    }
                    else if( reader->name() == "protocol" )
                    {
                        reader->readNext();
                        protocols << reader->text().toString();
                        reader->readNext();
                    }
                    else if( reader->name() == "restriction")
                    {
                        restriction = readRestriction( reader );
                        m_restrictionsList << restriction;
                    }
                    else if( reader->name() == "imageSet")
                    {
                        imageSet = readImageSet( reader );
                        hangingProtocol->addImageSet( imageSet );
                    }
                    else if( reader->name() == "displaySet")
                    {
                        displaySet = readDisplaySet( reader );
                        hangingProtocol->addDisplaySet( displaySet );
                    }
                    else if( reader->name() == "hangingProtocol" && reader->tokenType() == QXmlStreamReader::EndElement )
                    {
                        hangingProtocol->setProtocolsList( protocols );
                        listHangingProtocols.push_back( hangingProtocol );
                        break;
                    }
                }
            }
        }

        if (reader->hasError()) {
            DEBUG_LOG( tr("[Line: %1, Column:%2] Error in hanging protocol file %3: %4, error: %5").arg( reader->lineNumber() ).arg( reader->columnNumber() ).arg(path).arg( reader->errorString()).arg( reader->error()) );
        }
    }

    return listHangingProtocols;
}

HangingProtocolImageSet::Restriction HangingProtocolXMLReader::readRestriction( QXmlStreamReader * reader )
{
    HangingProtocolImageSet::Restriction restriction;

    while( !reader->atEnd() )
    {
        reader->readNext();
        reader->readNext();

        if( reader->name() == "usageFlag" )
        {
            reader->readNext();
            if( reader->text() == "MATCH" )
                restriction.usageFlag = HangingProtocolImageSet::MATCH;
            else if ( reader->text() == "NO_MATCH" )
                restriction.usageFlag = HangingProtocolImageSet::NO_MATCH;
        }
        else if( reader->name() == "selectorAttribute" )
        {
            reader->readNext();
            restriction.selectorAttribute = reader->text().toString();
        }
        else if( reader->name() == "valueRepresentation" )
        {
            reader->readNext();
            restriction.valueRepresentation = reader->text().toString();
        }
        else if( reader->name() == "restriction" && reader->tokenType() == QXmlStreamReader::EndElement )
        {
            break;
        }
        reader->readNext();// Fi de l'element
    }
    return restriction;
}

HangingProtocolImageSet * HangingProtocolXMLReader::readImageSet( QXmlStreamReader * reader )
{

    HangingProtocolImageSet * imageSet = new HangingProtocolImageSet();
    HangingProtocolImageSet::Restriction restriction;
    imageSet->setIdentifier( reader->attributes().value( "identifier").toString().toInt() );

    while( !reader->atEnd() )
    {
        reader->readNext();
        reader->readNext(); // Llegeix intro
        if( reader->name() == "restriction" )
        {
            reader->readNext();
            restriction = m_restrictionsList.value( reader->text().toString().toInt()-1 );
            imageSet->addRestriction( restriction );
        }
		else if( reader->name() == "type")
		{
			reader->readNext();
			imageSet->setTypeOfItem( reader->text().toString() );
		}
        else if( reader->name() == "imageSet" && reader->tokenType() == QXmlStreamReader::EndElement )
        {
            break;
        }

        reader->readNext();
    }
    return imageSet;
}

HangingProtocolDisplaySet * HangingProtocolXMLReader::readDisplaySet( QXmlStreamReader * reader )
{

    HangingProtocolDisplaySet * displaySet = new HangingProtocolDisplaySet();
    displaySet->setIdentifier( reader->attributes().value( "identifier").toString().toInt() );

    while( !reader->atEnd() )
    {
        reader->readNext();
        reader->readNext(); // Llegeix intro
        if( reader->name() == "imageSetNumber" )
        {
            reader->readNext();
            displaySet->setImageSetNumber( reader->text().toString().toInt() );
        }
        else if( reader->name() == "position" )
        {
            reader->readNext();
            displaySet->setPosition( reader->text().toString() );
        }
		else if( reader->name() == "patientOrientation" )
        {
            reader->readNext();
			displaySet->setPatientOrientation( reader->text().toString() );
        }
		else if( reader->name() == "reconstruction" )
		{
			reader->readNext();
			displaySet->setReconstruction( reader->text().toString() );
		}
		else if( reader->name() == "phase" )
		{
			reader->readNext();
			displaySet->setPhase( reader->text().toString() );
		}
        else if( reader->name() == "displaySet" && reader->tokenType() == QXmlStreamReader::EndElement )
        {
            break;
        }

        reader->readNext();
    }
    return displaySet;
}
}
#endif
