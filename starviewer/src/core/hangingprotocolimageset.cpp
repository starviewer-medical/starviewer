/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolimageset.h"
#include "logging.h"

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

void HangingProtocolImageSet::show()
{
    DEBUG_LOG( tr("    Identifier %1\n    List of restrictions:\n").arg(m_identifier) );

    for( int i = 0; i < m_listOfRestrictions.size(); i ++)
    {
        HangingProtocolImageSet::Restriction restriction = m_listOfRestrictions.value(i);
        DEBUG_LOG( tr("        Usage flag: %1\n        Selector attribute: %2\n        Value representation: %3\n        selectorValueNumber: %4\n").arg(restriction.usageFlag).arg(restriction.selectorAttribute).arg(restriction.valueRepresentation).arg(restriction.selectorValueNumber) );
    }
}


}

