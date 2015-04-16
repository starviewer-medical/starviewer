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

#include "hangingprotocol.h"

#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "series.h"
#include "logging.h"

namespace udg {

HangingProtocol::HangingProtocol()
{
    m_layout = new HangingProtocolLayout();
    m_mask = new HangingProtocolMask();
    m_strictness = false;
    m_allDifferent = false;
    m_numberOfPriors = 0;
    m_priority = 1;
}

HangingProtocol::HangingProtocol(const HangingProtocol &hangingProtocol)
    : m_identifier(hangingProtocol.m_identifier), m_name(hangingProtocol.m_name),
      m_institutionsRegularExpression(hangingProtocol.m_institutionsRegularExpression), m_strictness(hangingProtocol.m_strictness),
      m_allDifferent(hangingProtocol.m_allDifferent), m_iconType(hangingProtocol.m_iconType), m_numberOfPriors(hangingProtocol.m_numberOfPriors),
      m_priority(hangingProtocol.m_priority)
{
    m_layout = new HangingProtocolLayout(*hangingProtocol.m_layout);
    m_mask = new HangingProtocolMask(*hangingProtocol.m_mask);

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol.m_imageSets)
    {
        HangingProtocolImageSet *copiedImageSet = new HangingProtocolImageSet(*imageSet);
        copiedImageSet->setHangingProtocol(this);
        m_imageSets[copiedImageSet->getIdentifier()] = copiedImageSet;
    }

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol.m_displaySets)
    {
        HangingProtocolDisplaySet *copiedDisplaySet = new HangingProtocolDisplaySet(*displaySet);
        copiedDisplaySet->setHangingProtocol(this);
        copiedDisplaySet->setImageSet(this->getImageSet(displaySet->getImageSet()->getIdentifier()));
        m_displaySets[copiedDisplaySet->getIdentifier()] = copiedDisplaySet;
    }
}

HangingProtocol::~HangingProtocol()
{
    foreach (HangingProtocolImageSet *imageSet, m_imageSets)
    {
        delete imageSet;
    }
    m_imageSets.clear();

    foreach (HangingProtocolDisplaySet *displaySet, m_displaySets)
    {
        delete displaySet;
    }
    m_displaySets.clear();

    delete m_layout;
    delete m_mask;
}

void HangingProtocol::setName(const QString &name)
{
    m_name = name;
}

QString HangingProtocol::getName() const
{
    return m_name;
}

HangingProtocolLayout* HangingProtocol::getHangingProtocolLayout() const
{
    return m_layout;
}

HangingProtocolMask* HangingProtocol::getHangingProtocolMask() const
{
    return m_mask;
}

void HangingProtocol::setNumberOfScreens(int screens)
{
    m_layout->setNumberOfScreens(screens);
}

void HangingProtocol::setDisplayEnvironmentSpatialPositionList(const QStringList &positions)
{
    m_layout->setDisplayEnvironmentSpatialPositionList(positions);
}

void HangingProtocol::setProtocolsList(const QStringList &protocols)
{
    m_mask->setProtocolsList(protocols);
}

void HangingProtocol::addImageSet(HangingProtocolImageSet *imageSet)
{
    m_imageSets[imageSet->getIdentifier()] = imageSet;
    imageSet->setHangingProtocol(this);
}

void HangingProtocol::addDisplaySet(HangingProtocolDisplaySet *displaySet)
{
    m_displaySets[displaySet->getIdentifier()] = displaySet;
    displaySet->setHangingProtocol(this);
}

int HangingProtocol::getNumberOfImageSets() const
{
    return m_imageSets.size();
}

int HangingProtocol::getNumberOfDisplaySets() const
{
    return m_displaySets.size();
}

QList<HangingProtocolImageSet*> HangingProtocol::getImageSets() const
{
    return m_imageSets.values();
}

QList<HangingProtocolDisplaySet*> HangingProtocol::getDisplaySets() const
{
    return m_displaySets.values();
}

HangingProtocolImageSet* HangingProtocol::getImageSet(int identifier) const
{
    return m_imageSets[identifier];
}

HangingProtocolDisplaySet* HangingProtocol::getDisplaySet(int identifier) const
{
    return m_displaySets[identifier];
}

void HangingProtocol::setInstitutionsRegularExpression(const QRegExp &institutionRegularExpression)
{
    m_institutionsRegularExpression = institutionRegularExpression;
}

QRegExp HangingProtocol::getInstitutionsRegularExpression() const
{
    return m_institutionsRegularExpression;
}

void HangingProtocol::show() const
{
    DEBUG_LOG(QString("\n---- HANGING PROTOCOL ----\n Name: %1\n").arg(m_name));

    DEBUG_LOG("List of protocols: \n");
    for (int i = 0; i < m_mask->getProtocolList().size(); i++)
    {
        DEBUG_LOG(QString("%1, \n").arg(m_mask->getProtocolList().value(i)));
    }

    DEBUG_LOG("Institutions regular expression: " + m_institutionsRegularExpression.pattern() + "\n");

    DEBUG_LOG("List of image sets: \n");

    foreach (HangingProtocolImageSet *imageSet, m_imageSets)
    {
        imageSet->show();
    }

    DEBUG_LOG("List of display sets: \n");

    foreach (HangingProtocolDisplaySet *displaySet, m_displaySets)
    {
        displaySet->show();
    }
}

void HangingProtocol::setIdentifier(int id)
{
    m_identifier = id;
}

int HangingProtocol::getIdentifier() const
{
    return m_identifier;
}

bool HangingProtocol::isBetterThan(const HangingProtocol *hangingToCompare) const
{
    if (hangingToCompare == NULL)
    {
        return true;
    }

    // 1. Choose greatest priority
    if (this->getPriority() != hangingToCompare->getPriority())
    {
        return this->getPriority() > hangingToCompare->getPriority();
    }

    int thisFilledImageSets = this->countFilledImageSets();
    int thatFilledImageSets = hangingToCompare->countFilledImageSets();

    // 2. Choose greatest number of image sets with image
    if (thisFilledImageSets != thatFilledImageSets)
    {
        return thisFilledImageSets > thatFilledImageSets;
    }

    int thisFilledDisplaySets = this->countFilledDisplaySets();
    int thatFilledDisplaySets = hangingToCompare->countFilledDisplaySets();

    // 3. Choose greatest number of display sets with image
    if (thisFilledDisplaySets != thatFilledDisplaySets)
    {
        return thisFilledDisplaySets > thatFilledDisplaySets;
    }

    // 4. Choose greatest ratio of display sets with image, i.e. least number of empty display sets
    //    Since the number of display sets with image is the same, this is equivalent to least number of display sets
    if (this->getNumberOfDisplaySets() != hangingToCompare->getNumberOfDisplaySets())
    {
        return this->getNumberOfDisplaySets() < hangingToCompare->getNumberOfDisplaySets();
    }

    // 5. Choose the other
    return false;
}

int HangingProtocol::countFilledImageSets() const
{
    int count = 0;
    foreach (HangingProtocolImageSet *imageSet, this->getImageSets())
    {
        if (imageSet->getSeriesToDisplay() || imageSet->getPreviousStudyToDisplay())
        {
            count++;
        }
    }

    return count;
}

int HangingProtocol::countFilledImageSetsWithPriors() const
{
    int count = 0;
    foreach (HangingProtocolImageSet *imageSet, this->getImageSets())
    {
        if (imageSet->getAbstractPriorValue() != 0 && (imageSet->getSeriesToDisplay() || imageSet->getPreviousStudyToDisplay()))
        {
            count++;
        }
    }

    return count;
}

int HangingProtocol::countFilledDisplaySets() const
{
    int count = 0;
    foreach (HangingProtocolDisplaySet *displaySet, this->getDisplaySets())
    {
        if (displaySet->getImageSet() && displaySet->getImageSet()->getSeriesToDisplay())
        {
            if (displaySet->getSlice() != -1)
            {
                if (displaySet->getImageSet()->getSeriesToDisplay()->getNumberOfImages() > displaySet->getSlice())
                {
                    count++;
                }
            }
            else
            {
                count++;
            }
        }
    }

    return count;
}

bool HangingProtocol::isStrict() const
{
    return m_strictness;
}

void HangingProtocol::setStrictness(bool strictness)
{
    m_strictness = strictness;
}

void HangingProtocol::setAllDifferent(bool allDifferent)
{
    m_allDifferent = allDifferent;
}

bool HangingProtocol::getAllDifferent() const
{
    return m_allDifferent;
}

void HangingProtocol::setIconType(const QString &iconType)
{
    m_iconType = iconType;
}

QString HangingProtocol::getIconType() const
{
    return m_iconType;
}

void HangingProtocol::setNumberOfPriors(int numberOfPriors)
{
    m_numberOfPriors = numberOfPriors;
}

int HangingProtocol::getNumberOfPriors() const
{
    return m_numberOfPriors;
}

void HangingProtocol::setPriority(double priority)
{
    m_priority = priority;
}

double HangingProtocol::getPriority() const
{
    return m_priority;
}

bool HangingProtocol::compareTo(const HangingProtocol &hangingProtocol) const
{
    bool hasSameAttributes = m_identifier == hangingProtocol.getIdentifier()
        && m_name == hangingProtocol.getName()
        && m_strictness == hangingProtocol.isStrict()
        && m_allDifferent == hangingProtocol.getAllDifferent()
        && m_iconType == hangingProtocol.getIconType()
        && m_numberOfPriors == hangingProtocol.getNumberOfPriors()
        && m_priority == hangingProtocol.getPriority()
        && m_layout->getDisplayEnvironmentSpatialPositionList() == hangingProtocol.m_layout->getDisplayEnvironmentSpatialPositionList()
        && m_layout->getHorizontalPixelsList() == hangingProtocol.m_layout->getHorizontalPixelsList()
        && m_layout->getNumberOfScreens() == hangingProtocol.m_layout->getNumberOfScreens()
        && m_layout->getVerticalPixelsList() == hangingProtocol.m_layout->getVerticalPixelsList()
        && m_mask->getProtocolList() == hangingProtocol.m_mask->getProtocolList()
        && getImageSets().size() == hangingProtocol.getImageSets().size()
        && getDisplaySets().size() == hangingProtocol.getDisplaySets().size()
        && m_institutionsRegularExpression == hangingProtocol.getInstitutionsRegularExpression();

    int numberOfImageSets = getImageSets().size();
    int imageSetNumber = 0;

    while (hasSameAttributes && imageSetNumber < numberOfImageSets)
    {
        HangingProtocolImageSet *imageSet = getImageSets().at(imageSetNumber);
        HangingProtocolImageSet *imageSetToCompare = hangingProtocol.getImageSets().at(imageSetNumber);

        hasSameAttributes = imageSet->getIdentifier() == imageSetToCompare->getIdentifier()
            && imageSet->getTypeOfItem() == imageSetToCompare->getTypeOfItem()
            && imageSet->getSeriesToDisplay() == imageSetToCompare->getSeriesToDisplay()
            && imageSet->getImageToDisplay() == imageSetToCompare->getImageToDisplay()
            && imageSet->isDownloaded() == imageSetToCompare->isDownloaded()
            && imageSet->getPreviousStudyToDisplay() == imageSetToCompare->getPreviousStudyToDisplay()
            && imageSet->getImageNumberInStudyModality() == imageSetToCompare->getImageNumberInStudyModality();

        imageSetNumber++;
    }

    int numberOfDisplaySets = getDisplaySets().size();
    int displaySetNumber = 0;

    while (hasSameAttributes && displaySetNumber < numberOfDisplaySets)
    {
        HangingProtocolDisplaySet *displaySet = getDisplaySets().at(displaySetNumber);
        HangingProtocolDisplaySet *displaySetToCompare = hangingProtocol.getDisplaySets().at(displaySetNumber);

        hasSameAttributes = displaySet->getIdentifier() == displaySetToCompare->getIdentifier()
            && displaySet->getDescription() == displaySetToCompare->getDescription()
            && displaySet->getPosition() == displaySetToCompare->getPosition()
            //&& displaySet->getPatientOrientation() == displaySetToCompare->getPatientOrientation()
            && displaySet->getReconstruction() == displaySetToCompare->getReconstruction()
            && displaySet->getPhase() == displaySetToCompare->getPhase()
            && displaySet->getSlice() == displaySetToCompare->getSlice()
            && displaySet->getIconType() == displaySetToCompare->getIconType()
            && displaySet->getAlignment() == displaySetToCompare->getAlignment()
            && displaySet->getToolActivation() == displaySetToCompare->getToolActivation()
            && displaySet->getWindowWidth() == displaySetToCompare->getWindowWidth()
            && displaySet->getWindowCenter() == displaySetToCompare->getWindowCenter()
            && displaySet->getImageSet()->getIdentifier() == displaySetToCompare->getImageSet()->getIdentifier();

        displaySetNumber++;
    }
   
    return hasSameAttributes;
}

}
