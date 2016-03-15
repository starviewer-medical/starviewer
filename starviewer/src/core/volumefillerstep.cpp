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

#include "volumefillerstep.h"

#include "image.h"
#include "patientfillerinput.h"
#include "series.h"
#include "thumbnailcreator.h"

#include <QFileInfo>

namespace udg {

namespace {

// Ens diu si les imatges són de mides diferents
bool areOfDifferentSize(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    return firstImage->getColumns() != secondImage->getColumns() || firstImage->getRows() != secondImage->getRows();
}

// Ens diu si les imatges tenen photometric interpretations diferents
bool areOfDifferentPhotometricInterpretation(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    return firstImage->getPhotometricInterpretation() != secondImage->getPhotometricInterpretation();
}

// Ens diu si les imatges tenen pixel spacing diferents
bool areOfDifferentPixelSpacing(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    PixelSpacing2D spacing1 = firstImage->getPreferredPixelSpacing();
    PixelSpacing2D spacing2 = secondImage->getPreferredPixelSpacing();

    return !spacing1.isEqual(spacing2);
}

}

VolumeFillerStep::VolumeFillerStep()
{
}

bool VolumeFillerStep::fillIndividually()
{
    QList<Image*> currentImages = m_input->getCurrentImages();
    int numberOfFrames = currentImages.size();

    if (numberOfFrames == 0)
    {
        return false;
    }

    Image *image = currentImages.first();
    int currentImageIndex = m_input->getCurrentSeries()->getImages().indexOf(image);
    int volumeNumber = m_input->getCurrentVolumeNumber();
    // We will need to create a thumbnail if this is the first image in a volume, i.e. if it's the first in the series or we increase the volume number
    bool mustCreateThumbnail = currentImageIndex == 0;

    // If this is not the first image in the series we may need to increase the volume number
    if (currentImageIndex > 0)
    {
        // If the series contains at least one file with more than one frame, we increase the volume number for each fill
        if (m_input->currentSeriesContainsMultiframeImages())
        {
            volumeNumber++;
            mustCreateThumbnail = true;
        }
        // Otherwise, we increase the volume number if the current image has different size, color or spacing than the previous
        else
        {
            Image *previousImage = m_input->getCurrentSeries()->getImages().at(currentImageIndex - 1);

            if (areOfDifferentSize(previousImage, image) || areOfDifferentPhotometricInterpretation(previousImage, image)
                || areOfDifferentPixelSpacing(previousImage, image))
            {
                volumeNumber++;
                mustCreateThumbnail = true;
            }
        }
    }

    m_input->setCurrentVolumeNumber(volumeNumber);

    for (int i = 0; i < numberOfFrames; i++)
    {
        image = currentImages.at(i);
        image->setVolumeNumberInSeries(volumeNumber);
    }

    if (mustCreateThumbnail)
    {
        saveThumbnail(currentImages.first());
    }

    return true;
}

void VolumeFillerStep::saveThumbnail(const Image *image)
{
    int volumeNumber = m_input->getCurrentVolumeNumber();
    QString thumbnailPath = QFileInfo(image->getPath()).absolutePath();

    ThumbnailCreator thumbnailCreator;
    QImage thumbnail = thumbnailCreator.getThumbnail(image);
    thumbnail.save(QString("%1/thumbnail%2.png").arg(thumbnailPath).arg(volumeNumber), "PNG");

    // Si és el primer thumbnail, també creem el thumbnail ordinari que s'havia fet sempre
    if (volumeNumber == 1)
    {
        thumbnail.save(QString("%1/thumbnail.png").arg(thumbnailPath), "PNG");
    }
}

} // namespace udg
