/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocoldisplayset.h"
#include "logging.h"
#include "hangingprotocol.h"
#include "hangingprotocolimageset.h"

namespace udg {

HangingProtocolDisplaySet::HangingProtocolDisplaySet(QObject *parent)
 : QObject(parent)
{
    m_hangingProtocol = NULL;
    m_imageSet = NULL;
    m_sliceNumber = -1;
    m_phase = -1;
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

void HangingProtocolDisplaySet::setImageSet(HangingProtocolImageSet *imageSet)
{
    m_imageSet = imageSet;
}

HangingProtocolImageSet* HangingProtocolDisplaySet::getImageSet() const
{
    return m_imageSet;
}

void HangingProtocolDisplaySet::setHangingProtocol(HangingProtocol *hangingProtocol)
{
    m_hangingProtocol = hangingProtocol;
}


HangingProtocol* HangingProtocolDisplaySet::getHangingProtocol() const
{
    return m_hangingProtocol;
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

void HangingProtocolDisplaySet::setPhase( int phase )
{
    m_phase = phase;
}

int HangingProtocolDisplaySet::getPhase() const
{
    return m_phase;
}

void HangingProtocolDisplaySet::show()
{
    int imagSetIdentifier = -1;
    if (m_imageSet)
    {
        imagSetIdentifier = m_imageSet->getIdentifier();
    }

    DEBUG_LOG( QString("    Identifier %1\n    Description:%2\n    ImageSetNumber: %3\n    Position: %4\n").arg(m_identifier).arg(m_description).arg(imagSetIdentifier).arg(m_position) );
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
