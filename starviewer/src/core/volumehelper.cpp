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

#include "volumehelper.h"

#include "image.h"
#include "patient.h"
#include "volume.h"

namespace udg {

bool VolumeHelper::isPrimaryCT(Volume *volume)
{
    if (!volume)
    {
        return false;
    }

    Image *firstImage = volume->getImage(0);
    if (!firstImage)
    {
        return false;
    }

    return volume->getModality() == "CT" && firstImage->getImageType().contains("PRIMARY");
}

bool VolumeHelper::isPrimaryPET(Volume *volume)
{
    if (!volume)
    {
        return false;
    }

    Image *firstImage = volume->getImage(0);
    if (!firstImage)
    {
        return false;
    }
    
    return volume->getModality() == "PT" && firstImage->getImageType().contains("PRIMARY");
}

bool VolumeHelper::isPrimaryNM(Volume *volume)
{
    if (!volume)
    {
        return false;
    }

    Image *firstImage = volume->getImage(0);
    if (!firstImage)
    {
        return false;
    }

    return (volume->getModality() == "NM" && firstImage->getImageType().contains("PRIMARY"));
}

void VolumeHelper::generatePatientVolumes(Patient *patient)
{
    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getViewableSeries())
        {
            using VolumeNumber = int;
            QMap<VolumeNumber, QList<Image*>> volumesImages;    // map from each volume to its images

            foreach (Image *image, series->getImages())
            {
                VolumeNumber volumeNumber = image->getVolumeNumberInSeries();
                volumesImages[volumeNumber].append(image);  // this will create a new item (a list) at volumeNumber if it doesn't exist
            }

            foreach (auto imageList, volumesImages)
            {
                // Count phases assuming that all phases for each slice are consecutive images and that first phase is numbered 0
                // We only need to find the second occurrence of phase 0 in the list

                Q_ASSERT(!imageList.isEmpty());
                Q_ASSERT(imageList[0]->getPhaseNumber() == 0);

                int i = 1;

                while (i < imageList.size() && imageList[i]->getPhaseNumber() > 0)
                {
                    i++;
                }

                int numberOfPhases = i;
                int numberOfSlicesPerPhase = imageList.size() / numberOfPhases;

                Volume *volume = new Volume();
                volume->setImages(imageList);
                volume->setNumberOfPhases(numberOfPhases);
                volume->setNumberOfSlicesPerPhase(numberOfSlicesPerPhase);
                volume->setThumbnail(imageList.at(imageList.count() / 2)->getThumbnail(true));
                series->addVolume(volume);
            }
        }
    }

    DEBUG_LOG(QString("Patient:\n%1").arg(patient->toString()));
}

} // End namespace udg
