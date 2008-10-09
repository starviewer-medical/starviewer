/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolimageset.h"
#include "logging.h"
#include "series.h"

namespace udg {

HangingProtocolImageSet::HangingProtocolImageSet(QObject *parent)
 : QObject(parent)
{
	m_serieToDisplay = NULL;
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


void HangingProtocolImageSet::setTypeOfItem( QString type)
{
	m_typeOfItem = type;
}

QString HangingProtocolImageSet::getTypeOfItem()
{
	return m_typeOfItem;
}

void HangingProtocolImageSet::setImageToDisplay( int imageNumber )
{
	m_imageToDisplay = imageNumber;
}

int HangingProtocolImageSet::getImatgeToDisplay()
{	
	return m_imageToDisplay;
}

void HangingProtocolImageSet::setSeriesToDisplay( Series * series )
{
	m_serieToDisplay = series;
}

Series * HangingProtocolImageSet::getSeriesToDisplay()
{
	return m_serieToDisplay;
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

