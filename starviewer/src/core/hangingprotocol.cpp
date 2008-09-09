/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocol.h"

#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "logging.h"

namespace udg {

HangingProtocol::HangingProtocol(QObject *parent)
 : QObject( parent )
{
    m_layout = new HangingProtocolLayout();
    m_mask = new HangingProtocolMask();
}


HangingProtocol::~HangingProtocol()
{
}

HangingProtocolLayout * HangingProtocol::getHangingProtocolLayout()
{
    return m_layout;
}

HangingProtocolMask * HangingProtocol::getHangingProtocolMask()
{
    return m_mask;
}

void HangingProtocol::setNumberOfScreens( int screens )
{
    m_layout->setNumberOfScreens( screens );
}

void HangingProtocol::setDisplayEnvironmentSpatialPositionList( QList<QString> positions )
{
    m_layout->setDisplayEnvironmentSpatialPositionList( positions );
}

void HangingProtocol::setProtocolsList( QList<QString> protocols )
{
    m_mask->setProtocolsList( protocols );
}

void HangingProtocol::addImageSet ( HangingProtocolImageSet * imageSet )
{
    m_listOfImageSets.push_back( imageSet );
}

void HangingProtocol::addDisplaySet ( HangingProtocolDisplaySet * displaySet )
{
    m_listOfDisplaySets.push_back( displaySet );
}

int HangingProtocol::getNumberOfImageSets()
{
    return m_listOfImageSets.size();
}

HangingProtocolImageSet * HangingProtocol::getImageSet( int identificador )
{
    HangingProtocolImageSet * imageSet = 0;
    bool found = false;
    int i = 0;
    int numberOfImageSets = m_listOfImageSets.size();

    while( !found && i < numberOfImageSets )
    {
        if( m_listOfImageSets.value( i )->getIdentifier() == identificador )
        {
            found = true;
            imageSet = m_listOfImageSets.value( i );
        }
        i++;
    }

    return imageSet;
}

HangingProtocolDisplaySet * HangingProtocol::getDisplaySetOfImageSet( int numberOfImageSet )
{

    HangingProtocolDisplaySet * displaySet = 0;
    bool found = false;
    int i = 0;
    int numberOfDisplaySets = m_listOfDisplaySets.size();

    while( !found && i < numberOfDisplaySets )
    {
        if( m_listOfDisplaySets.value( i )->getImageSetNumber() == numberOfImageSet )
        {
            found = true;
            displaySet = m_listOfDisplaySets.value( i );
        }
        i++;
    }

    return displaySet;

}

void HangingProtocol::show()
{
    DEBUG_LOG( tr("\n---- HANGING PROTOCOL ----\n Name: %1\nDescription: %2\nLevel: %3\nCreator: %4\nDate: %5\n").arg(m_name).arg(m_description).arg(m_level).arg(m_creator).arg(m_dateTime) );

    DEBUG_LOG( tr("List of protocols: \n") );
    for( int i = 0; i < m_mask->getProtocolList().size(); i++ )
    {
        DEBUG_LOG( tr("%1, \n").arg(m_mask->getProtocolList().value( i )) );
    }

    DEBUG_LOG( tr("List of image sets: \n") );

    for( int i = 0; i < m_listOfImageSets.size(); i++ )
    {
        m_listOfImageSets.value( i )->show();
    }

    DEBUG_LOG( tr("List of display sets: \n") );

    for( int i = 0; i < m_listOfDisplaySets.size(); i++ )
    {
        m_listOfDisplaySets.value( i )->show();
    }
}


}
