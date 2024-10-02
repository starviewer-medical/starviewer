/*************************************************************************************
  Copyright (C) 2024 Laboratori de Gràfics i Imatge, Universitat de Girona &
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

#include "ordervolumesfillerstep.h"

#include "image.h"
#include "patient.h"
#include "patientfillerinput.h"
#include "utils.h"

#include <QFileInfo>

namespace udg {

namespace {

// Ensures a deterministic order for the volumes in the given series, ordering them following the criteria specified in a comment below.
void processSeries(Series *series)
{
    const QList<Image*> &images = series->getImages();

    if (images.isEmpty())
    {
        return;
    }

    Q_ASSERT(images.first()->getVolumeNumberInSeries() == 1);

    int numberOfVolumes = images.last()->getVolumeNumberInSeries();

    if (numberOfVolumes == 1)
    {
        return;
    }

    // Gather data needed for sorting

    struct SeriesVolume
    {
        int volumeNumber;
        const Image *firstImage;
        int rangeBegin;
        int rangeEnd;
    };

    QVector<SeriesVolume> seriesVolumes;
    seriesVolumes.reserve(numberOfVolumes);
    bool sortByImageTime = true;        // will remain true if all representative images have it
    bool sortByInstanceNumber = true;   // will remain true if all representative images have it

    for (int volumeNumber = 1, imageIndex = 0; volumeNumber <= numberOfVolumes; volumeNumber++)
    {
        int rangeBegin = imageIndex;
        const Image *firstImage = images[rangeBegin];
        sortByImageTime &= !firstImage->getImageTime().isEmpty();
        sortByInstanceNumber &= !firstImage->getInstanceNumber().isEmpty();

        while (imageIndex < images.size() && images[imageIndex]->getVolumeNumberInSeries() == volumeNumber)
        {
            imageIndex++;
        }

        int rangeEnd = imageIndex;

        seriesVolumes.append({ volumeNumber, firstImage, rangeBegin, rangeEnd });
    }

    // Sort

    std::sort(seriesVolumes.begin(), seriesVolumes.end(), [=](const SeriesVolume &item1, const SeriesVolume &item2) {
        // Criteria:
        //  1. Image time (DICOM Content Time) (if both have it)
        //  2. Instance Number (if both have it)
        //  3. SOP Instance UID
        const Image *image1 = item1.firstImage;
        const Image *image2 = item2.firstImage;

        if (sortByImageTime && image1->getImageTime() != image2->getImageTime())
        {
            return image1->getImageTime() < image2->getImageTime();
        }

        if (sortByInstanceNumber && image1->getInstanceNumber().toInt() != image2->getInstanceNumber().toInt())
        {
            return image1->getInstanceNumber().toInt() < image2->getInstanceNumber().toInt();
        }

        return Utils::compareUids(image1->getSOPInstanceUID(), image2->getSOPInstanceUID()) < 0;
    });

    // Check for changes

    bool changed = false;
    QHash<int, int> volumeMapping;
    volumeMapping.reserve(numberOfVolumes);

    for (int i = 0; i < numberOfVolumes; i++)
    {
        changed |= seriesVolumes[i].volumeNumber != i + 1;
        volumeMapping[seriesVolumes[i].volumeNumber] = i + 1;
    }

    if (!changed)
    {
        return;
    }

    // Apply new order

    QList<Image*> reorderedImages;
    reorderedImages.reserve(images.size());
    int newVolumeNumber = 1;

    for (const SeriesVolume &sv : seriesVolumes)
    {
        for (int i = sv.rangeBegin; i < sv.rangeEnd; i++)
        {
            reorderedImages.append(images[i]);
            reorderedImages.last()->setVolumeNumberInSeries(newVolumeNumber);
        }

        newVolumeNumber++;
    }

    series->setImages(reorderedImages);

    // Rename thumbnails

    QString thumbnailPath = QFileInfo(images.first()->getPath()).absolutePath();    // TODO coupling with path and thumbnail name
    QString thumbnailPattern = thumbnailPath + "/thumbnail%1.png";

    for (int volumeNumber = 1; volumeNumber <= numberOfVolumes; volumeNumber++)
    {
        QString currentName = thumbnailPattern.arg(volumeNumber);

        if (QFile::exists(currentName))
        {
            QFile::rename(currentName, currentName + ".tmp");
        }
    }

    for (int volumeNumber = 1; volumeNumber <= numberOfVolumes; volumeNumber++)
    {
        QString currentName = thumbnailPattern.arg(volumeNumber) + ".tmp";

        if (QFile::exists(currentName))
        {
            QString newName = thumbnailPattern.arg(volumeMapping[volumeNumber]);
            QFile::rename(currentName, newName);
        }
    }
}

}

bool OrderVolumesFillerStep::fillIndividually()
{
    return true;
}

void OrderVolumesFillerStep::postProcessing()
{
    for (const Patient *patient : m_input->getPatientList())
    {
        for (const Study *study : patient->getStudies())
        {
            for (Series *series : study->getSeries())
            {
                processSeries(series);
            }
        }
    }
}

} // namespace udg
