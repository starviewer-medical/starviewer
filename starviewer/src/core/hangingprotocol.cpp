/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocol.h"

#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "series.h"
#include "logging.h"

namespace udg {

HangingProtocol::HangingProtocol(QObject *parent)
 : QObject(parent)
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
    m_level = hangingProtocol->m_level;
    m_creator = hangingProtocol->m_creator;
    m_dateTime = hangingProtocol->m_dateTime;
    m_candidate = hangingProtocol->m_candidate;
    m_strictness = hangingProtocol->m_strictness;
    m_allDiferent = hangingProtocol->m_allDiferent;
    m_iconType = hangingProtocol->m_iconType;
    m_hasPrevious = hangingProtocol->m_hasPrevious;
    m_priority =  hangingProtocol->m_priority;

    // Copia del layout
    m_layout = new HangingProtocolLayout();
    m_layout->setDisplayEnvironmentSpatialPositionList(hangingProtocol->m_layout->getDisplayEnvironmentSpatialPositionList());
    m_layout->setHorizontalPixelsList(hangingProtocol->m_layout->getHorizontalPixelsList());
    m_layout->setNumberOfScreens(hangingProtocol->m_layout->getNumberOfScreens());
    m_layout->setVerticalPixelsList(hangingProtocol->m_layout->getVerticalPixelsList());

    //Copia de la mascara
    m_mask = new HangingProtocolMask();
    m_mask->setProtocolsList(hangingProtocol->m_mask->getProtocolList());

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol->m_listOfImageSets)
    {
        HangingProtocolImageSet *copiedImageSet = new HangingProtocolImageSet();
        copiedImageSet->setRestrictions(imageSet->getRestrictions());
        copiedImageSet->setIdentifier(imageSet->getIdentifier());
        copiedImageSet->setTypeOfItem(imageSet->getTypeOfItem());
        copiedImageSet->setSeriesToDisplay(imageSet->getSeriesToDisplay());
        copiedImageSet->setImageToDisplay(imageSet->getImageToDisplay());
        copiedImageSet->setIsPreviousStudy(imageSet->isPreviousStudy());
        copiedImageSet->setDownloaded(imageSet->isDownloaded());
        copiedImageSet->setPreviousStudyToDisplay(imageSet->getPreviousStudyToDisplay());
        copiedImageSet->setPreviousStudyPacs(imageSet->getPreviousStudyPacs());
        copiedImageSet->setPreviousImageSetReference(imageSet->getPreviousImageSetReference());
        copiedImageSet->setImageNumberInPatientModality(imageSet->getImageNumberInPatientModality());
        copiedImageSet->setHangingProtocol(this);
        m_listOfImageSets.append(copiedImageSet);
    }

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol->m_listOfDisplaySets)
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
        m_listOfDisplaySets.append(copiedDisplaySet);
    }

}

HangingProtocol::~HangingProtocol()
{
    foreach (HangingProtocolImageSet *imageSet, m_listOfImageSets)
    {
        if (imageSet)
        {
            delete imageSet;
        }
    }
    m_listOfImageSets.clear();

    foreach (HangingProtocolDisplaySet *displaySet, m_listOfDisplaySets)
    {
        if (displaySet)
        {
            delete displaySet;
        }
    }
    m_listOfDisplaySets.clear();

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

void HangingProtocol::setDisplayEnvironmentSpatialPositionList(const QList<QString> &positions)
{
    m_layout->setDisplayEnvironmentSpatialPositionList(positions);
}

void HangingProtocol::setProtocolsList(const QList<QString> &protocols)
{
    m_mask->setProtocolsList(protocols);
}

void HangingProtocol::addImageSet(HangingProtocolImageSet *imageSet)
{
    m_listOfImageSets.push_back(imageSet);
    imageSet->setHangingProtocol(this);
}

void HangingProtocol::addDisplaySet(HangingProtocolDisplaySet *displaySet)
{
    m_listOfDisplaySets.push_back(displaySet);
    displaySet->setHangingProtocol(this);
}

int HangingProtocol::getNumberOfImageSets() const
{
    return m_listOfImageSets.size();
}

int HangingProtocol::getNumberOfDisplaySets() const
{
    return m_listOfDisplaySets.size();
}

QList<HangingProtocolImageSet*> HangingProtocol::getImageSets() const
{
    return m_listOfImageSets;
}

QList<HangingProtocolDisplaySet*> HangingProtocol::getDisplaySets() const
{
    return m_listOfDisplaySets;
}

HangingProtocolImageSet* HangingProtocol::getImageSet(int identifier)
{
    HangingProtocolImageSet *imageSet = 0;
    bool found = false;
    int i = 0;
    int numberOfImageSets = m_listOfImageSets.size();

    while (!found && i < numberOfImageSets)
    {
        if (m_listOfImageSets.value(i)->getIdentifier() == identifier)
        {
            found = true;
            imageSet = m_listOfImageSets.value(i);
        }
        i++;
    }

    return imageSet;
}

HangingProtocolDisplaySet* HangingProtocol::getDisplaySet(int identifier) const
{
    HangingProtocolDisplaySet *displaySet = 0;
    bool found = false;
    int i = 0;
    int numberOfDisplaySets = m_listOfDisplaySets.size();

    while (!found && i < numberOfDisplaySets)
    {
        if (m_listOfDisplaySets.value(i)->getIdentifier() == identifier)
        {
            found = true;
            displaySet = m_listOfDisplaySets.value(i);
        }
        i++;
    }

    return displaySet;
}

void HangingProtocol::show()
{
    DEBUG_LOG(QString("\n---- HANGING PROTOCOL ----\n Name: %1\nDescription: %2\nLevel: %3\nCreator: %4\nDate: %5\n").arg(m_name).arg(m_description).arg(m_level).arg(m_creator).arg(m_dateTime));

    DEBUG_LOG("List of protocols: \n");
    for (int i = 0; i < m_mask->getProtocolList().size(); i++)
    {
        DEBUG_LOG(QString("%1, \n").arg(m_mask->getProtocolList().value(i)));
    }

    DEBUG_LOG("List of image sets: \n");

    for (int i = 0; i < m_listOfImageSets.size(); i++)
    {
        m_listOfImageSets.value(i)->show();
    }

    DEBUG_LOG("List of display sets: \n");

    for (int i = 0; i < m_listOfDisplaySets.size(); i++)
    {
        m_listOfDisplaySets.value(i)->show();
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

    if (this->getPriority() != -1 && hangingToCompare->getPriority() != -1)
    {
        // Si tots 2 tenen prioritat definida els fem competir
        return this->getPriority() > hangingToCompare->getPriority();
    }
    else
    {
        //Si un des 2 hangings no té la prioritat definida, la prioritat només serveix  
        //per dir si un hanging ha de ser el més o el menys aconsellat.
        if (this->getPriority() == 10 || hangingToCompare->getPriority() == 0)
        {
            return true;
        }

        if (this->getPriority() == 0 || hangingToCompare->getPriority() == 10)
        {
            return false;
        }
    }

    if (this->countFilledDisplaySets() == hangingToCompare->countFilledDisplaySets())
    {
        if (this->countFilledDisplaySets() / (double)this->getNumberOfDisplaySets() == hangingToCompare->countFilledDisplaySets() / (double)hangingToCompare->getNumberOfDisplaySets())
        {
            if(this->getNumberOfImageSets() != hangingToCompare->getNumberOfImageSets())
            {
                return (this->getNumberOfImageSets() > hangingToCompare->getNumberOfImageSets());
            }
        }
        else
        {
            return this->countFilledDisplaySets() / (double)this->getNumberOfDisplaySets() > hangingToCompare->countFilledDisplaySets() / (double)hangingToCompare->getNumberOfDisplaySets();
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
    foreach(HangingProtocolImageSet *imageSet, this->getImageSets())
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

bool HangingProtocol::isPrevious()
{
    return m_hasPrevious;
}

void HangingProtocol::setPriority(double priority)
{
    m_priority = priority;
}

double HangingProtocol::getPriority()
{
    return m_priority;
}

}
