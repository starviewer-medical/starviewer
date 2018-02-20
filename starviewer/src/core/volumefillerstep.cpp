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

VolumeFillerStep::VolumeFillerStep(bool dontCreateThumbnails)
    : m_dontCreateThumbnails(dontCreateThumbnails)
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
    int volumeNumber = m_input->getCurrentVolumeNumber();
    bool mustCreateThumbnail = false;
    Series *series = m_input->getCurrentSeries();
    ImageProperties imageProperties(image);

    if (numberOfFrames > 1)
    {
        // This file has more than one frame: let's create a volume for it alone
        imageProperties.multiframe = true;

        if (m_imagesProperties.contains(series) && !m_imagesProperties[series].isEmpty())
        {
            // Give it a new volumeNumber
            volumeNumber = m_imagesProperties[series].lastKey() + 1;
        }
        // else: First file of the series: no need to increment volumeNumber

        m_imagesProperties[series][volumeNumber] = imageProperties;
        mustCreateThumbnail = true;
    }
    else if (m_imagesProperties.contains(series) && m_imagesProperties[series].contains(volumeNumber))
    {
        // This file has one frame
        // Current volumeNumber already contains images: let's find if they match the current one
        if (m_imagesProperties[series][volumeNumber] != imageProperties)
        {
            // No match: let's find if there is some matching ImageProperties in current series
            bool found = false;
            foreach (int number, m_imagesProperties[series].keys())
            {
                if (m_imagesProperties[series][number] == imageProperties)
                {
                    // Found: reuse the volumeNumber
                    found = true;
                    volumeNumber = number;
                    break;
                }
            }

            if (!found)
            {
                // Not found: insert imageProperties with a new volumeNumber
                volumeNumber = m_imagesProperties[series].lastKey() + 1;
                m_imagesProperties[series][volumeNumber] = imageProperties;
                mustCreateThumbnail = true;
            }
        }
        // else Match: no need to change volumeNumber nor create thumbnail
    }
    else
    {
        // First image of this volume: insert imageProperties at current series and volumeNumber
        m_imagesProperties[series][volumeNumber] = imageProperties;
        mustCreateThumbnail = true;
    }

    m_input->setCurrentVolumeNumber(volumeNumber);

    for (int i = 0; i < numberOfFrames; i++)
    {
        image = currentImages.at(i);
        image->setVolumeNumberInSeries(volumeNumber);
    }

    if (!m_dontCreateThumbnails && mustCreateThumbnail)
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

VolumeFillerStep::ImageProperties::ImageProperties(const Image *image)
    : multiframe(false), rows(image->getRows()), columns(image->getColumns()), photometricInterpretation(image->getPhotometricInterpretation()),
      pixelSpacing(image->getPreferredPixelSpacing())
{
}

bool VolumeFillerStep::ImageProperties::operator==(const ImageProperties &that) const
{
    // Multiframes are always considered different to avoid finding them in the search for a matching ImageProperties
    return !this->multiframe && !that.multiframe && this->rows == that.rows && this->columns == that.columns
            && this->photometricInterpretation == that.photometricInterpretation && this->pixelSpacing.isEqual(that.pixelSpacing);
}

bool VolumeFillerStep::ImageProperties::operator!=(const ImageProperties &that) const
{
    return !(*this == that);
}

} // namespace udg
