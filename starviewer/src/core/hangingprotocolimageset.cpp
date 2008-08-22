/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolimageset.h"

namespace udg {

HangingProtocolImageSet::HangingProtocolImageSet(QObject *parent)
 : QObject(parent)
{
}


HangingProtocolImageSet::~HangingProtocolImageSet()
{
}

void HangingProtocolImageSet::setIdentifier( int identifier)
{
    m_identifier = identifier;
}

int HangingProtocolImageSet::getIdentifier()
{
    return m_identifier;
}

void HangingProtocolImageSet::addRestriction( Restriction restriction )
{
    m_listOfRestrictions << restriction;
}

QList<HangingProtocolImageSet::Restriction> HangingProtocolImageSet::getRestrictions()
{
    return m_listOfRestrictions;
}

}

