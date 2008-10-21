/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocoldisplayset.h"
#include "logging.h"

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

void HangingProtocolDisplaySet::setPatientOrientation( QString orientation )
{
	m_patientOrientation = orientation;
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

QString HangingProtocolDisplaySet::getPatientOrientation()
{
	return m_patientOrientation;
}

void HangingProtocolDisplaySet::setReconstruction( QString reconstruction )
{
	m_reconstruction = reconstruction;
}

QString HangingProtocolDisplaySet::getReconstruction()
{
	return m_reconstruction;
}

void HangingProtocolDisplaySet::setPhase( QString phase )
{
	m_phase = phase;
}
		
QString HangingProtocolDisplaySet::getPhase()
{
	return m_phase;
}

void HangingProtocolDisplaySet::show()
{
    DEBUG_LOG( tr("    Identifier %1\n    Description:%2\n    ImageSetNumber: %3\n    Position: %4\n").arg(m_identifier).arg(m_description).arg(m_imageSetNumber).arg(m_position) );

}
}
