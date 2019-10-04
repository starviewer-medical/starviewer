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

#ifndef UDGORDERIMAGESFILLERSTEP_H
#define UDGORDERIMAGESFILLERSTEP_H

#include "patientfillerstep.h"

#include <QHash>
#include <QMap>

namespace udg {

class Image;

/**
 * @brief The OrderImagesFillerStep class orders images inside each volume.
 *
 * To achieve this it decides the order and then calls setOrderNumberInVolume() for each image and setImages() for each series (so that the series has the list
 * of images in the correct order).
 *
 * Images that can be spatially sorted are separated by acquisition number and stack id, then sorted by Image::distance inside each group; then stacks inside
 * each acquisition are sorted by minimum Image::distance and finally acquisitions are also sorted by minimum Image::distance. Images with the same position are
 * sorted with the same criteria as images that can't be spatially sorted, according to a set of more or less abstract values; these are, in order of
 * preference, dimension index values, instance number SOP instance UID, and frame number.
 *
 * For more information read qms://doc/065875b1.
 */
class OrderImagesFillerStep : public PatientFillerStep
{
public:
    OrderImagesFillerStep();
    ~OrderImagesFillerStep() override;

    bool fillIndividually() override;
    void postProcessing() override;

    /// Returns true if the given volume of the given series can be spatially sorted. It's public to allow testing.
    bool canBeSpatiallySorted(Series *series, int volume) const;

    /// Returns true if the Image::distance of \a image1 is lower than that of \a image2, and false if it is greater. If they are equal it calls and returns the
    /// value of lesserAbstractValues. It is used as the comparator function of std::sort. It's public to allow testing.
    static bool lesserSpatialPosition(const Image *image1, const Image *image2);

    /// Returns true if \a image1 must be ordered before \a image2 according to several abstract values (see class description or qms://doc/065875b1). It is
    /// used as the comparator function of std::sort. It's public to allow testing.
    static bool lesserAbstractValues(const Image *image1, const Image *image2);

private:
    /// Sorts spatially the given list of images. See class description or qms://doc/065875b1 for criteria.
    static void spatialSort(QList<Image*> &images);

    /// Sorts the given list images according to lesserAbstractValues.
    static void basicSort(QList<Image*> &images);

private:
    /**
     * @brief A hash that stores an Image for each encountered position and orientation.
     *
     * The key is a string constructed from the position and the orientation.
     */
    struct SampleImagePerPosition : public QHash<QString, Image*>
    {
        /// Returns the key to be used to insert the given image into the hash, created from its position and orientation.
        static QString hashKey(const Image *image);
        /// Updates this hash with the given image.
        void add(Image *image);
    };

    /// Stores an instance of SampleImagePerPosition for each series and volume.
    QHash<Series*, QHash<int, SampleImagePerPosition>> m_sampleImagePerPositionPerVolume;
    /// Stores the list of images in each series and volume, for easy access. The inner one is a map to have volume numbers ordered.
    QHash<Series*, QMap<int, QList<Image*>>> m_imagesPerVolume;

};

}

#endif
