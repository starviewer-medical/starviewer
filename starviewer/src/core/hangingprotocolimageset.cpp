/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "hangingprotocolimageset.h"
#include "logging.h"
#include "series.h"
#include "study.h"
#include "hangingprotocol.h"
#include "hangingprotocolimagesetrestriction.h"

namespace udg {

HangingProtocolImageSet::HangingProtocolImageSet()
 : m_type(Type::Series), m_abstractPriorValue(0)
{
    m_hangingProtocol = NULL;
    m_previousStudyToDisplay = NULL;
    m_downloaded = true;
    m_imageNumberInStudyModality = -1;
}

HangingProtocolImageSet::~HangingProtocolImageSet()
{
}

void HangingProtocolImageSet::setIdentifier(int identifier)
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

const QVector<HangingProtocolImageSetRestrictionExpression>& HangingProtocolImageSet::getRestrictionExpressions() const
{
    return m_restrictionExpressions;
}

void HangingProtocolImageSet::setRestrictionExpressions(const QVector<HangingProtocolImageSetRestrictionExpression> &restrictionExpressions)
{
    m_restrictionExpressions = restrictionExpressions;
}

void HangingProtocolImageSet::addRestrictionExpression(const HangingProtocolImageSetRestrictionExpression &restrictionExpression)
{
    m_restrictionExpressions.append(restrictionExpression);
}

int HangingProtocolImageSet::getNumberOfRestrictionExpressions() const
{
    return m_restrictionExpressions.size();
}

void HangingProtocolImageSet::setType(Type type)
{
    m_type = type;
}

HangingProtocolImageSet::Type HangingProtocolImageSet::getType() const
{
    return m_type;
}

void HangingProtocolImageSet::setImageToDisplay(int imageNumber)
{
    m_imageToDisplay = imageNumber;
}

int HangingProtocolImageSet::getImageToDisplay() const
{
    return m_imageToDisplay;
}

void HangingProtocolImageSet::addSeriesToDisplay(Series *series)
{
    m_seriesToDisplay.append(series);
}

void HangingProtocolImageSet::clearSeriesToDisplay()
{
    m_seriesToDisplay.clear();
}

const QVector<Series*>& HangingProtocolImageSet::getSeriesToDisplay() const
{
    return m_seriesToDisplay;
}

int HangingProtocolImageSet::getNumberOfSeriesToDisplay() const
{
    return m_seriesToDisplay.size();
}

void HangingProtocolImageSet::show()
{
    DEBUG_LOG(QString("    Identifier %1\n").arg(m_identifier));
}

void HangingProtocolImageSet::setDownloaded(bool option)
{
    m_downloaded = option;
}

bool HangingProtocolImageSet::isDownloaded()
{
    return m_downloaded;
}

void HangingProtocolImageSet::setPreviousStudyToDisplay(Study *study)
{
    m_previousStudyToDisplay = study;
}

Study* HangingProtocolImageSet::getPreviousStudyToDisplay()
{
    return m_previousStudyToDisplay;
}

int HangingProtocolImageSet::getAbstractPriorValue() const
{
    return m_abstractPriorValue;
}

void HangingProtocolImageSet::setAbstractPriorValue(int value)
{
    m_abstractPriorValue = value;
}

int HangingProtocolImageSet::getImageNumberInStudyModality()
{
    return m_imageNumberInStudyModality;
}

void HangingProtocolImageSet::setImageNumberInStudyModality(int imageNumberInStudyModality)
{
    m_imageNumberInStudyModality = imageNumberInStudyModality;
}

bool HangingProtocolImageSet::isFilled() const
{
    return getNumberOfSeriesToDisplay() == getNumberOfRestrictionExpressions();
}

}
