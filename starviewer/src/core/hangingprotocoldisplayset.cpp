/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocoldisplayset.h"

namespace udg {

HangingProtocolDisplaySet::HangingProtocolDisplaySet(QObject *parent)
 : QObject(parent)
{
}


HangingProtocolDisplaySet::~HangingProtocolDisplaySet()
{
}


void HangingProtocolDisplaySet::setIdentifier( int identifier )
{
    m_identifier = identifier;
}

void HangingProtocolDisplaySet::setDescription( QString description )
{
    m_description = description;
}

void HangingProtocolDisplaySet::setImageSetNumber( int number )
{
    m_imageSetNumber = number;
}

void HangingProtocolDisplaySet::setPosition( QString position )
{
    m_position = position;
}

int HangingProtocolDisplaySet::getIdentifier()
{
    return m_identifier;
}

QString HangingProtocolDisplaySet::getDescription()
{
    return m_description;
}

int HangingProtocolDisplaySet::getImageSetNumber()
{
    return m_imageSetNumber;
}

QString HangingProtocolDisplaySet::getPosition()
{
    return m_position;
}

}
