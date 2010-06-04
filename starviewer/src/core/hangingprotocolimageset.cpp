/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolimageset.h"
#include "logging.h"
#include "series.h"
#include "study.h"
#include "hangingprotocol.h"

namespace udg {

HangingProtocolImageSet::HangingProtocolImageSet(QObject *parent)
 : QObject(parent)
{
    m_hangingProtocol = NULL;
    m_previousStudyToDisplay = NULL;
    m_serieToDisplay = NULL;
    m_isPreviousStudy = false;
    m_downloaded = true;
}

HangingProtocolImageSet::~HangingProtocolImageSet()
{
}

void HangingProtocolImageSet::setIdentifier( int identifier)
{
    m_identifier = identifier;
}

int HangingProtocolImageSet::getIdentifier() const
{
    return m_identifier;
}

void HangingProtocolImageSet::setHangingProtocol(HangingProtocol *hangingProtocol)
{
    m_hangingProtocol = hangingProtocol;
}


HangingProtocol* HangingProtocolImageSet::getHangingProtocol() const
{
    return m_hangingProtocol;
}

void HangingProtocolImageSet::addRestriction( Restriction restriction )
{
    m_listOfRestrictions << restriction;
}

QList<HangingProtocolImageSet::Restriction> HangingProtocolImageSet::getRestrictions() const
{
    return m_listOfRestrictions;
}

void HangingProtocolImageSet::setTypeOfItem( QString type)
{
    m_typeOfItem = type;
}

QString HangingProtocolImageSet::getTypeOfItem() const
{
    return m_typeOfItem;
}

void HangingProtocolImageSet::setImageToDisplay( int imageNumber )
{
    m_imageToDisplay = imageNumber;
}

int HangingProtocolImageSet::getImageToDisplay() const
{	
    return m_imageToDisplay;
}

void HangingProtocolImageSet::setSeriesToDisplay( Series * series )
{
    m_serieToDisplay = series;
}

Series * HangingProtocolImageSet::getSeriesToDisplay() const
{
    return m_serieToDisplay;
}

void HangingProtocolImageSet::show()
{
    DEBUG_LOG( QString("    Identifier %1\n    List of restrictions:\n").arg(m_identifier) );

    for( int i = 0; i < m_listOfRestrictions.size(); i ++)
    {
        HangingProtocolImageSet::Restriction restriction = m_listOfRestrictions.value(i);
        DEBUG_LOG( QString("        Usage flag: %1\n        Selector attribute: %2\n        Value representation: %3\n        selectorValueNumber: %4\n").arg(restriction.usageFlag).arg(restriction.selectorAttribute).arg(restriction.valueRepresentation).arg(restriction.selectorValueNumber) );
    }
}

void HangingProtocolImageSet::setIsPreviousStudy( bool hasPreviousStudy )
{
    m_isPreviousStudy = hasPreviousStudy;
}

bool HangingProtocolImageSet::isPreviousStudy()
{
    return m_isPreviousStudy;
}

void HangingProtocolImageSet::setDownloaded( bool option )
{
	m_downloaded = option;
}

bool HangingProtocolImageSet::isDownloaded()
{
	return m_downloaded;
}

void HangingProtocolImageSet::setPreviousStudyToDisplay( Study * study )
{
    m_previousStudyToDisplay = study;
}

Study * HangingProtocolImageSet::getPreviousStudyToDisplay()
{
	return m_previousStudyToDisplay;
}

void HangingProtocolImageSet::setPreviousStudyPacs( QString pacs )
{
    m_previousStudyPacs = pacs;
}

QString HangingProtocolImageSet::getPreviousStudyPacs()
{
    return m_previousStudyPacs;
}

void HangingProtocolImageSet::setPreviousImageSetReference( int imageSetNumber )
{
    m_previousImageSetReference = imageSetNumber;
}

int HangingProtocolImageSet::getPreviousImageSetReference()
{
    return m_previousImageSetReference;
}



}

