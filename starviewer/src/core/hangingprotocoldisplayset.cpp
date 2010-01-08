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
    m_sliceNumber = -1;
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

int HangingProtocolDisplaySet::getIdentifier() const
{
    return m_identifier;
}

QString HangingProtocolDisplaySet::getDescription() const
{
    return m_description;
}

int HangingProtocolDisplaySet::getImageSetNumber() const
{
    return m_imageSetNumber;
}

QString HangingProtocolDisplaySet::getPosition() const
{
    return m_position;
}

QString HangingProtocolDisplaySet::getPatientOrientation() const
{
    return m_patientOrientation;
}

void HangingProtocolDisplaySet::setReconstruction( QString reconstruction )
{
    m_reconstruction = reconstruction;
}

QString HangingProtocolDisplaySet::getReconstruction() const
{
    return m_reconstruction;
}

void HangingProtocolDisplaySet::setPhase( QString phase )
{
    m_phase = phase;
}

QString HangingProtocolDisplaySet::getPhase() const
{
    return m_phase;
}

void HangingProtocolDisplaySet::show()
{
    DEBUG_LOG( QString("    Identifier %1\n    Description:%2\n    ImageSetNumber: %3\n    Position: %4\n").arg(m_identifier).arg(m_description).arg(m_imageSetNumber).arg(m_position) );
}

void HangingProtocolDisplaySet::setSlice( int sliceNumber )
{
    m_sliceNumber = sliceNumber;
}

int HangingProtocolDisplaySet::getSlice()
{
    return m_sliceNumber;
}

void HangingProtocolDisplaySet::setIconType( QString iconType )
{
    m_iconType = iconType;
}

QString HangingProtocolDisplaySet::getIconType() const
{
    return m_iconType;
}

void HangingProtocolDisplaySet::setAlignment( QString alignment )
{
    m_alignment = alignment;
}

QString HangingProtocolDisplaySet::getAlignment() const
{
    return m_alignment;
}

void HangingProtocolDisplaySet::setToolActivation( QString toolActive )
{
    m_activateTool = toolActive;
}

QString HangingProtocolDisplaySet::getToolActivation()
{
    return m_activateTool;
}

}
