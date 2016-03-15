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

#include "hangingprotocolfiller.h"

#include "hangingprotocol.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocolmask.h"
#include "image.h"
#include "logging.h"
#include "patient.h"
#include "study.h"

namespace udg {

namespace {

// Search the image by index from the list of images of the study that match the given modalities
Image* getImageByIndexInStudyModality(const Study *study, int index, const QStringList &hangingProtocolModalities)
{
    QList<Image*> allImagesInStudy;

    // TODO Es podria millorar amb una cerca fins a la imatge que està a l'índex, envers d'un recorregut agafant-les totes

    foreach (Series *series, study->getSeries())
    {
        if (hangingProtocolModalities.contains(series->getModality()))
        {
            allImagesInStudy.append(series->getImages());
        }
    }

    if (index < allImagesInStudy.size())
    {
        return allImagesInStudy.at(index);
    }
    else
    {
        return 0;
    }
}

// Cert si la imatge compleix les restriccions
bool isValidImage(const Image *image, const HangingProtocolImageSet *imageSet)
{
    if (!image)
    {
        DEBUG_LOG("La imatge passada és NUL·LA! Retornem fals.");
        return false;
    }

    return imageSet->getRestrictionExpression().test(image);
}

// This method is to temporally preserve backwards compatibility with the imageNumberInStudyModality tag. Should be removed when it isn't needed anymore.
void fillImageSetWithImageNumberInStudyModality(HangingProtocolImageSet *imageSet, const Study *currentStudy)
{
    QStringList modalities = imageSet->getHangingProtocol()->getHangingProtocolMask()->getProtocolList();
    Image *image = getImageByIndexInStudyModality(currentStudy, imageSet->getImageNumberInStudyModality(), modalities);

    if (isValidImage(image, imageSet))
    {
        Series *selectedSeries = image->getParentSeries();
        imageSet->setImageToDisplay(selectedSeries->getImages().indexOf(image));
        imageSet->setSeriesToDisplay(selectedSeries);
    }
    else
    {
        // Segur que no hi ha cap més imatge vàlida
        // Important, no hi posem cap serie!
        imageSet->setSeriesToDisplay(0);
        imageSet->setImageToDisplay(0);
    }
}

// Returns the studies from priorStudies that are older than currentStudy and have one of the required modalities.
QList<Study*> getPriorStudies(const Study *currentStudy, const QList<Study*> &priorStudies, const QStringList &modalities)
{
    QList<Study*> studies = Study::sortStudies(priorStudies, Study::RecentStudiesFirst);
    QSet<QString> wantedModalities = modalities.toSet();
    QMutableListIterator<Study*> it(studies);

    while (it.hasNext())
    {
        Study *study = it.next();

        if (study->getDateTime() >= currentStudy->getDateTime() || study->getModalities().toSet().intersect(wantedModalities).isEmpty())
        {
            it.remove();
        }
    }

    return studies;
}

// Returns the applicable study for the given image set according to its abstract prior value, chosen from the current study and the prior studies.
Study* getCurrentOrPriorStudy(const HangingProtocolImageSet *imageSet, Study *currentStudy, const QList<Study*> &allPriorStudies)
{
    if (imageSet->getAbstractPriorValue() == 0)
    {
        return currentStudy;
    }

    QList<Study*> priorStudies = getPriorStudies(currentStudy, allPriorStudies, imageSet->getHangingProtocol()->getHangingProtocolMask()->getProtocolList());

    if (!priorStudies.isEmpty())
    {
        if (imageSet->getAbstractPriorValue() == -1)
        {
            return priorStudies.last();
        }
        else if (imageSet->getAbstractPriorValue() <= priorStudies.size())
        {
            return priorStudies[imageSet->getAbstractPriorValue() - 1];
        }
    }

    return 0;
}

// Returns true if the given hanging protocol is applicable to the given modality.
bool isModalityCompatible(const HangingProtocol *protocol, const QString &modality)
{
    return protocol->getHangingProtocolMask()->getProtocolList().contains(modality);
}

}

void HangingProtocolFiller::fill(HangingProtocol *hangingProtocol, Study *currentStudy, const QList<Study*> &priorStudies)
{
    m_usedSeries.clear();

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol->getImageSets())
    {
        fillImageSet(imageSet, currentStudy, priorStudies);
    }
}

void HangingProtocolFiller::fillImageSetWithStudy(HangingProtocolImageSet *imageSet, const Study *study)
{
    m_usedSeries.clear();
    findUsedSeries(imageSet->getHangingProtocol());
    fillImageSetWithStudyPrivate(imageSet, study);
}

void HangingProtocolFiller::fillImageSet(HangingProtocolImageSet *imageSet, Study *currentStudy, const QList<Study*> &priorStudies)
{
    // Pot ser que busquem una imatge en concret, llavors no cal examinar totes les sèries i/o totes les imatges
    // Només pot ser vàlida una imatge
    if (imageSet->getImageNumberInStudyModality() != -1)
    {
        // HACK! This shall be removed in the future.
        fillImageSetWithImageNumberInStudyModality(imageSet, currentStudy);
        return;
    }

    Study *study = getCurrentOrPriorStudy(imageSet, currentStudy, priorStudies);

    if (!study)
    {
        return;
    }

    fillImageSetWithStudyPrivate(imageSet, study);
}

void HangingProtocolFiller::fillImageSetWithStudyPrivate(HangingProtocolImageSet *imageSet, const Study *study)
{
    foreach (Series *series, study->getSeries())
    {
        if (fillImageSetWithSeries(imageSet, series))
        {
            return;
        }
    }
}

bool HangingProtocolFiller::fillImageSetWithSeries(HangingProtocolImageSet *imageSet, Series *series)
{
    if (imageSet->getHangingProtocol()->getAllDifferent() && m_usedSeries.contains(series))
    {
        return false;
    }

    if (!isModalityCompatible(imageSet->getHangingProtocol(), series->getModality()))
    {
        return false;
    }

    if (imageSet->getTypeOfItem() != "image" && !imageSet->getRestrictionExpression().test(series))
    {
        return false;
    }
    else
    {
        bool found = false;

        for (int i = 0; i < series->getNumberOfImages() && !found; i++)
        {
            const Image *image = series->getImageByIndex(i);

            if (imageSet->getRestrictionExpression().test(image))
            {
                found = true;
                imageSet->setImageToDisplay(i);
            }
        }

        if (!found)
        {
            return false;
        }
    }

    imageSet->setSeriesToDisplay(series);
    m_usedSeries << series;

    return true;
}

void HangingProtocolFiller::findUsedSeries(HangingProtocol *hangingProtocol)
{
    foreach (HangingProtocolImageSet *imageSet, hangingProtocol->getImageSets())
    {
        const Series *series = imageSet->getSeriesToDisplay();

        if (series)
        {
            m_usedSeries << series;
        }
    }
}

} // namespace udg
