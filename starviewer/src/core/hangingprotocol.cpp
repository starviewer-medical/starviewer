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
    m_strictness = false;
    m_allDiferent = false;
    m_hasPrevious = false;
    m_hasStudiesToDownload = false;
    m_priority = -1;
}


HangingProtocol::~HangingProtocol()
{
}

void HangingProtocol::setName( QString name )
{
    m_name = name;
}

QString HangingProtocol::getName() const
{
    return m_name;
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

int HangingProtocol::getNumberOfImageSets() const
{
    return m_listOfImageSets.size();
}

int HangingProtocol::getNumberOfDisplaySets() const
{
    return m_listOfDisplaySets.size();
}

HangingProtocolImageSet * HangingProtocol::getImageSet( int identifier )
{
    HangingProtocolImageSet * imageSet = 0;
    bool found = false;
    int i = 0;
    int numberOfImageSets = m_listOfImageSets.size();

    while( !found && i < numberOfImageSets )
    {
        if( m_listOfImageSets.value( i )->getIdentifier() == identifier )
        {
            found = true;
            imageSet = m_listOfImageSets.value( i );
        }
        i++;
    }

    return imageSet;
}

HangingProtocolDisplaySet * HangingProtocol::getDisplaySet( int identifier ) const
{
    HangingProtocolDisplaySet * displaySet = 0;
    bool found = false;
    int i = 0;
    int numberOfDisplaySets = m_listOfDisplaySets.size();

    while( !found && i < numberOfDisplaySets )
    {
        if( m_listOfDisplaySets.value( i )->getIdentifier() == identifier )
        {
            found = true;
            displaySet = m_listOfDisplaySets.value( i );
        }
        i++;
    }

    return displaySet;
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
    DEBUG_LOG( QString("\n---- HANGING PROTOCOL ----\n Name: %1\nDescription: %2\nLevel: %3\nCreator: %4\nDate: %5\n").arg(m_name).arg(m_description).arg(m_level).arg(m_creator).arg(m_dateTime) );

    DEBUG_LOG( "List of protocols: \n" );
    for( int i = 0; i < m_mask->getProtocolList().size(); i++ )
    {
        DEBUG_LOG( QString("%1, \n").arg(m_mask->getProtocolList().value( i )) );
    }

    DEBUG_LOG( "List of image sets: \n" );

    for( int i = 0; i < m_listOfImageSets.size(); i++ )
    {
        m_listOfImageSets.value( i )->show();
    }

    DEBUG_LOG( "List of display sets: \n" );

    for( int i = 0; i < m_listOfDisplaySets.size(); i++ )
    {
        m_listOfDisplaySets.value( i )->show();
    }
}

void HangingProtocol::setIdentifier( int id )
{
    m_identifier = id;
}

int HangingProtocol::getIdentifier() const
{
    return m_identifier;
}

bool HangingProtocol::isBetterThan( HangingProtocol * hangingToCompare )
{
    int i;
    bool hasViewWithPatientOrientation_1 = false;
    bool hasViewWithPatientOrientation_2 = false;
    bool hasReconstruction_1 = false;
    bool hasReconstruction_2 = false;

    if( hangingToCompare == NULL ) return true;

    // Estratègia per saber quin té més prioritat que un altre.
    if( this->getPriority() != -1 )
    {
        if ( hangingToCompare->getPriority() != -1 )
            return this->getPriority()> hangingToCompare->getPriority(); // Tots dos tenen prioritat
        else  // El hanging a comparar no té prioritat
        {
            if( this->getPriority() == 10 ) // Si la prioritat és 10 és millor
                return true;
            else if ( this->getPriority() == 0 ) // Si la prioritat és 0, no el volem com a millor
                return false;

            // altrament continua mirant les condicions de més avall
        }
    }
    else //El hanging actual no té prioritat
    {
        if ( hangingToCompare->getPriority() != -1 ) //El hanging a comparar si que te prioritat
        {
            if( hangingToCompare->getPriority() == 10 ) // Si la prioritat és 10 és millor
                return false;
            else if ( hangingToCompare->getPriority() == 0 ) // Si la prioritat és 0, no el volem com a millor
                return true;

            // altrament continua mirant les condicions de més avall
        }
    }

    if( this->getNumberOfDisplaySets() != hangingToCompare->getNumberOfDisplaySets() )
        return ( this->getNumberOfDisplaySets() > hangingToCompare->getNumberOfDisplaySets() );
    else if( this->getNumberOfImageSets() != hangingToCompare->getNumberOfImageSets() )
        return ( this->getNumberOfImageSets() > hangingToCompare->getNumberOfImageSets() );
    else
    {
        i = 1;
        while( !hasViewWithPatientOrientation_1 && !hasViewWithPatientOrientation_1 && i <= this->getNumberOfDisplaySets() )
        {
            hasViewWithPatientOrientation_1 = !(this->getDisplaySet( i )->getPosition().isEmpty());
            i++;
        }

        i = 1;
        while( !hasViewWithPatientOrientation_2 && !hasViewWithPatientOrientation_2 && i <= hangingToCompare->getNumberOfDisplaySets() )
        {
            hasViewWithPatientOrientation_2 = !(hangingToCompare->getDisplaySet( i )->getPatientOrientation().isEmpty());
            i++;
        }

        if( hasViewWithPatientOrientation_1 == hasViewWithPatientOrientation_2)
        {
            if( hasReconstruction_1 == hasReconstruction_2 ) 
            {
                return false; /// Són iguals
            }
            else return hasReconstruction_1;
        }
        else return hasViewWithPatientOrientation_1;
    }
}

bool HangingProtocol::isStrict() const
{
    return m_strictness;
}

void HangingProtocol::setStrictness( bool strictness )
{
    m_strictness = strictness;
}

void HangingProtocol::setAllDiferent( bool allDiferent )
{
    m_allDiferent = allDiferent;
}

bool HangingProtocol::getAllDiferent() const
{
    return m_allDiferent;
}

void HangingProtocol::setIconType( QString iconType )
{
    m_iconType = iconType;
}

QString HangingProtocol::getIconType() const
{
    return m_iconType;
}

void HangingProtocol::setPrevious( bool isPrevious )
{
    m_hasPrevious = isPrevious;
}

bool HangingProtocol::isPrevious()
{
    return m_hasPrevious;
}

void HangingProtocol::setHasStudiesToDownload( bool hasStudiesToDownload )
{
    m_hasStudiesToDownload = hasStudiesToDownload;
}

bool HangingProtocol::hasStudiesToDownload()
{
    return m_hasStudiesToDownload;
}

void HangingProtocol::setPriority( double priority )
{
    m_priority = priority;
}

double HangingProtocol::getPriority()
{
    return m_priority;
}

}
