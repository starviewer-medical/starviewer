/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocollayout.h"

namespace udg {

HangingProtocolLayout::HangingProtocolLayout(QObject *parent)
: QObject( parent )
{
}


HangingProtocolLayout::~HangingProtocolLayout()
{
}

void HangingProtocolLayout::setNumberOfScreens( int numberOfScreens )
{
    m_numberOfScreens = numberOfScreens;
}

void HangingProtocolLayout::setVerticalPixelsList( QList<int> verticalPixelsList )
{
    m_verticalPixelsList = verticalPixelsList;
}

void HangingProtocolLayout::setHorizontalPixelsList( QList<int> horizontalPixelsList )
{
    m_horizontalPixelsList = horizontalPixelsList;
}

void HangingProtocolLayout::setDisplayEnvironmentSpatialPositionList( QList<QString> displayEnvironmentSpatialPosition )
{
    m_displayEnvironmentSpatialPosition = displayEnvironmentSpatialPosition;
}

int HangingProtocolLayout::getNumberOfScreens()
{
    return m_numberOfScreens;
}

QList<int> HangingProtocolLayout::getVerticalPixelsList()
{
    return m_verticalPixelsList;
}

QList<int> HangingProtocolLayout::getHorizontalPixelsList()
{
    return m_horizontalPixelsList;
}

QList<QString> HangingProtocolLayout::getDisplayEnvironmentSpatialPositionList()
{
    return m_displayEnvironmentSpatialPosition;
}

}
