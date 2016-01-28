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

#include "dicomtagreader.h"
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
    if (!m_input->getCurrentImages().isEmpty())
    {
        const DICOMTagReader *dicomReader = m_input->getDICOMFile();
        processDICOMFile(dicomReader);
    }

    return true;
}

QString VolumeFillerStep::name()
{
    return "VolumeFillerStep";
}

void VolumeFillerStep::processDICOMFile(const DICOMTagReader *dicomReader)
{
    int numberOfFrames = 1;
    int volumeNumber = m_input->getCurrentSingleFrameVolumeNumber();
    if (dicomReader->tagExists(DICOMNumberOfFrames))
    {
        numberOfFrames = dicomReader->getValueAttributeAsQString(DICOMNumberOfFrames).toInt();
        // Si és la segona imatge multiframe que ens trobem, augmentarem el número que identifica l'actual volum
        if (m_input->currentSeriesContainsAMultiframeVolume())
        {
            m_input->increaseCurrentMultiframeVolumeNumber();
        }
        volumeNumber = m_input->getCurrentMultiframeVolumeNumber();
    }

    QList<Image*> currentImages = m_input->getCurrentImages();

    for (int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++)
    {
        Image *image = currentImages.at(frameNumber);

        // Setting volume number

        // Comprovem si les imatges són de diferents mides per assignar-lis volums diferents
        // Això només passarà quan les imatges siguin single-frame
        if (numberOfFrames == 1)
        {
            int imagesInCurrentFile = currentImages.size();
            int imagesInCurrentSeries = m_input->getCurrentSeries()->getImages().size();

            if (imagesInCurrentSeries > imagesInCurrentFile)
            {
                // Si la imatge anterior i l'actual tenen mides diferents, aniran en un volum diferent
                Image *lastProcessedImage = m_input->getCurrentSeries()->getImages().at(imagesInCurrentSeries - imagesInCurrentFile - 1);
                if (areOfDifferentSize(lastProcessedImage, image) || areOfDifferentPhotometricInterpretation(lastProcessedImage, image)
                    || areOfDifferentPixelSpacing(lastProcessedImage, image))
                {
                    m_input->increaseCurrentSingleFrameVolumeNumber();
                    volumeNumber = m_input->getCurrentSingleFrameVolumeNumber();
                    // Actualitzem el número actual de volum i guardem el corresponent thumbnail
                    m_input->setCurrentVolumeNumber(volumeNumber);
                    // HACK Si és la segona imatge de mida diferent, cal generar el propi thumbnail de la imatge anterior
                    if (volumeNumber == 101)
                    {
                        QString path = QString("%1/thumbnail%2.png").arg(QFileInfo(lastProcessedImage->getPath()).absolutePath()).arg(
                                               lastProcessedImage->getVolumeNumberInSeries());
                        ThumbnailCreator().getThumbnail(lastProcessedImage).save(path, "PNG");
                    }
                    saveThumbnail(dicomReader);
                }
            }
        }
        image->setVolumeNumberInSeries(volumeNumber);
    }
    m_input->setCurrentVolumeNumber(volumeNumber);

    if (currentImages.count() > 1)
    {
        // Com que la imatge és multiframe (tant si és enhanced com si no) creem els corresponents thumbnails i els guardem a la cache
        saveThumbnail(dicomReader);
    }
}

void VolumeFillerStep::saveThumbnail(const DICOMTagReader *dicomReader)
{
    int volumeNumber = m_input->getCurrentVolumeNumber();
    QString thumbnailPath = QFileInfo(dicomReader->getFileName()).absolutePath();

    ThumbnailCreator thumbnailCreator;
    QImage thumbnail = thumbnailCreator.getThumbnail(dicomReader);
    thumbnail.save(QString("%1/thumbnail%2.png").arg(thumbnailPath).arg(volumeNumber), "PNG");

    // Si és el primer thumbnail, també creem el thumbnail ordinari que s'havia fet sempre
    if (volumeNumber == 1)
    {
        thumbnail.save(QString("%1/thumbnail.png").arg(thumbnailPath), "PNG");
    }
}

} // namespace udg
