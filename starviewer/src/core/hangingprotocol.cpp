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
    m_allDiferent = false;
    m_hasPrevious = false;
    m_priority = -1;
}

HangingProtocol::HangingProtocol(const HangingProtocol *hangingProtocol)
{
    m_identifier = hangingProtocol->m_identifier;
    m_name = hangingProtocol->m_name;
    m_description = hangingProtocol->m_description;
    m_strictness = hangingProtocol->m_strictness;
    m_allDiferent = hangingProtocol->m_allDiferent;
    m_iconType = hangingProtocol->m_iconType;
    m_hasPrevious = hangingProtocol->m_hasPrevious;
    m_priority = hangingProtocol->m_priority;
    m_institutionsRegularExpression = hangingProtocol->getInstitutionsRegularExpression();

    // Copia del layout
    m_layout = new HangingProtocolLayout();
    m_layout->setDisplayEnvironmentSpatialPositionList(hangingProtocol->m_layout->getDisplayEnvironmentSpatialPositionList());
    m_layout->setHorizontalPixelsList(hangingProtocol->m_layout->getHorizontalPixelsList());
    m_layout->setNumberOfScreens(hangingProtocol->m_layout->getNumberOfScreens());
    m_layout->setVerticalPixelsList(hangingProtocol->m_layout->getVerticalPixelsList());

    // Copia de la mascara
    m_mask = new HangingProtocolMask();
    m_mask->setProtocolsList(hangingProtocol->m_mask->getProtocolList());

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol->m_imageSets)
    {
        HangingProtocolImageSet *copiedImageSet = new HangingProtocolImageSet();
        copiedImageSet->setRestrictionExpression(imageSet->getRestrictionExpression());
        copiedImageSet->setIdentifier(imageSet->getIdentifier());
        copiedImageSet->setTypeOfItem(imageSet->getTypeOfItem());
        copiedImageSet->setSeriesToDisplay(imageSet->getSeriesToDisplay());
        copiedImageSet->setImageToDisplay(imageSet->getImageToDisplay());
        copiedImageSet->setIsPreviousStudy(imageSet->isPreviousStudy());
        copiedImageSet->setDownloaded(imageSet->isDownloaded());
        copiedImageSet->setPreviousStudyToDisplay(imageSet->getPreviousStudyToDisplay());
        copiedImageSet->setPreviousImageSetReference(imageSet->getPreviousImageSetReference());
        copiedImageSet->setImageNumberInPatientModality(imageSet->getImageNumberInPatientModality());
        copiedImageSet->setHangingProtocol(this);
        m_imageSets[copiedImageSet->getIdentifier()] = copiedImageSet;
    }

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol->m_displaySets)
    {
        HangingProtocolDisplaySet *copiedDisplaySet = new HangingProtocolDisplaySet();
        copiedDisplaySet->setIdentifier(displaySet->getIdentifier());
        copiedDisplaySet->setDescription(displaySet->getDescription());
        copiedDisplaySet->setPosition(displaySet->getPosition());
        copiedDisplaySet->setPatientOrientation(displaySet->getPatientOrientation());
        copiedDisplaySet->setReconstruction(displaySet->getReconstruction());
        copiedDisplaySet->setPhase(displaySet->getPhase());
        copiedDisplaySet->setSlice(displaySet->getSlice());
        copiedDisplaySet->setIconType(displaySet->getIconType());
        copiedDisplaySet->setAlignment(displaySet->getAlignment());
        copiedDisplaySet->setToolActivation(displaySet->getToolActivation());
        copiedDisplaySet->setHangingProtocol(this);
        copiedDisplaySet->setImageSet(this->getImageSet(displaySet->getImageSet()->getIdentifier()));
        copiedDisplaySet->setWindowWidth(displaySet->getWindowWidth());
        copiedDisplaySet->setWindowCenter(displaySet->getWindowCenter());
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

HangingProtocolLayout* HangingProtocol::getHangingProtocolLayout()
{
    return m_layout;
}

HangingProtocolMask* HangingProtocol::getHangingProtocolMask()
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

void HangingProtocol::show()
{
    DEBUG_LOG(QString("\n---- HANGING PROTOCOL ----\n Name: %1\nDescription: %2\n").arg(m_name).arg(
              m_description));

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

bool HangingProtocol::isBetterThan(HangingProtocol *hangingToCompare)
{
    if (hangingToCompare == NULL)
    {
        return true;
    }

    if (this->getPriority() != hangingToCompare->getPriority())
    {
        if (this->getPriority() != -1 && hangingToCompare->getPriority() != -1)
        {
            // Si tots 2 tenen prioritat definida els fem competir
            return this->getPriority() > hangingToCompare->getPriority();
        }
        else
        {
            // Si un des 2 hangings no té la prioritat definida, la prioritat només serveix
            // per dir si un hanging ha de ser el més o el menys aconsellat.
            if (this->getPriority() == 10 || hangingToCompare->getPriority() == 0)
            {
                return true;
            }

            if (this->getPriority() == 0 || hangingToCompare->getPriority() == 10)
            {
                return false;
            }
        }
    }

    if (this->countFilledDisplaySets() == hangingToCompare->countFilledDisplaySets())
    {
        if (this->countFilledDisplaySets() / (double)this->getNumberOfDisplaySets() == hangingToCompare->countFilledDisplaySets() /
           (double)hangingToCompare->getNumberOfDisplaySets())
        {
            if (this->getNumberOfImageSets() != hangingToCompare->getNumberOfImageSets())
            {
                return (this->getNumberOfImageSets() > hangingToCompare->getNumberOfImageSets());
            }
        }
        else
        {
            return this->countFilledDisplaySets() / (double)this->getNumberOfDisplaySets() > hangingToCompare->countFilledDisplaySets() /
                   (double)hangingToCompare->getNumberOfDisplaySets();
        }
    }
    else
    {
        return (this->countFilledDisplaySets() > hangingToCompare->countFilledDisplaySets());
    }

    return false;
}

int HangingProtocol::countFilledImageSets() const
{
    int count = 0;
    foreach (HangingProtocolImageSet *imageSet, this->getImageSets())
    {
        if (imageSet->getSeriesToDisplay())
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

void HangingProtocol::setAllDiferent(bool allDiferent)
{
    m_allDiferent = allDiferent;
}

bool HangingProtocol::getAllDiferent() const
{
    return m_allDiferent;
}

void HangingProtocol::setIconType(const QString &iconType)
{
    m_iconType = iconType;
}

QString HangingProtocol::getIconType() const
{
    return m_iconType;
}

void HangingProtocol::setPrevious(bool isPrevious)
{
    m_hasPrevious = isPrevious;
}

bool HangingProtocol::isPrevious() const
{
    return m_hasPrevious;
}

void HangingProtocol::setPriority(double priority)
{
    m_priority = priority;
}

double HangingProtocol::getPriority() const
{
    return m_priority;
}

bool HangingProtocol::compareTo(const HangingProtocol &hangingProtocol)
{
    bool hasSameAttributes = m_identifier == hangingProtocol.getIdentifier()
        && m_name == hangingProtocol.getName()
        && m_description == hangingProtocol.m_description
        && m_strictness == hangingProtocol.isStrict()
        && m_allDiferent == hangingProtocol.getAllDiferent()
        && m_iconType == hangingProtocol.getIconType()
        && m_hasPrevious == hangingProtocol.isPrevious()
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
            && imageSet->isPreviousStudy() == imageSetToCompare->isPreviousStudy()
            && imageSet->isDownloaded() == imageSetToCompare->isDownloaded()
            && imageSet->getPreviousStudyToDisplay() == imageSetToCompare->getPreviousStudyToDisplay()
            && imageSet->getImageNumberInPatientModality() == imageSetToCompare->getImageNumberInPatientModality();

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
