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

#include "orderimagesfillerstep.h"

#include "image.h"
#include "imageplane.h"
#include "logging.h"
#include "mathtools.h"
#include "patientfillerinput.h"
#include "series.h"

#include <functional>
#include <unordered_map>

#include <QtConcurrent>

#include <vtkPlane.h>

namespace udg {

OrderImagesFillerStep::OrderImagesFillerStep()
{
}

OrderImagesFillerStep::~OrderImagesFillerStep()
{
}

bool OrderImagesFillerStep::fillIndividually()
{
    QList<Image*> currentImages = m_input->getCurrentImages();

    if (!currentImages.isEmpty())
    {
        // If the hash contains no item with the key, the function inserts a default-constructed value into the hash with the key, and returns a reference to
        // it. Thus the first time each series and volume are processed new entries will be created but the next times they will be reused.
        SampleImagePerPosition &sampleImagePerPosition = m_sampleImagePerPositionPerVolume[m_input->getCurrentSeries()][m_input->getCurrentVolumeNumber()];
        QList<Image*> &volumeImages = m_imagesPerVolume[m_input->getCurrentSeries()][m_input->getCurrentVolumeNumber()];
        QSet<ImageOrientation> &volumeOrientations = m_orientationsPerVolume[m_input->getCurrentSeries()][m_input->getCurrentVolumeNumber()];

        foreach (Image *image, m_input->getCurrentImages())
        {
            sampleImagePerPosition.add(image);
            volumeImages.append(image);
            volumeOrientations.insert(image->getImageOrientationPatient());
        }
    }

    return true;
}

void OrderImagesFillerStep::postProcessing()
{
    foreach (Series *series, m_imagesPerVolume.keys())
    {
        QList<Image*> seriesImages;

        foreach (int volume, m_imagesPerVolume.value(series).keys())
        {
            QList<Image*> volumeImages = m_imagesPerVolume[series].take(volume);

            if (canBeSpatiallySorted(series, volume))
            {
                bool severalOrientations = m_orientationsPerVolume[series][volume].size() > 1;
                spatialSort(volumeImages, severalOrientations);
            }
            else
            {
                basicSort(volumeImages);
            }

            for (int i = 0; i < volumeImages.size(); i++)
            {
                volumeImages[i]->setOrderNumberInVolume(i);
            }

            seriesImages.append(volumeImages);
        }

        series->setImages(seriesImages);
    }
}

bool OrderImagesFillerStep::canBeSpatiallySorted(Series *series, int volume) const
{
    if (!series)
    {
        return false;
    }

    const SampleImagePerPosition &sampleImagePerPosition = m_sampleImagePerPositionPerVolume[series][volume];

    if (sampleImagePerPosition.size() <= 1)
    {
        DEBUG_LOG("Only phases or less than 2 images");
        return false;
    }

    // If we reach this point we know that we have at least two different ImagePlanes.
    // Now we need a list of different ImagePlanes sorted by Image::distance to later check for spatial consistence.

    QList<Image*> sampleImages = sampleImagePerPosition.values();

    std::sort(sampleImages.begin(), sampleImages.end(), [](const Image *image1, const Image *image2) {
        return Image::distance(image1) < Image::distance(image2);
    });

    std::function<ImagePlane(const Image*)> mapFunction = [](const Image *image)
    {
        ImagePlane imagePlane;
        imagePlane.fillFromImage(image);
        return imagePlane;
    };

    QList<ImagePlane> imagePlanes = QtConcurrent::blockingMapped(sampleImages, mapFunction);

    // Here we check for "spatial consistence". Two consecutive images are considered spatially consistent if a line perpendicular to the first image and
    // passing through its center intersects the rectangle of the second image.

    for (int i = 0; i < imagePlanes.size() - 1; i++)
    {
        const ImagePlane &imagePlane1 = imagePlanes[i];
        const ImagePlane &imagePlane2 = imagePlanes[i+1];

        Vector3 lineP1 = imagePlane1.getCenter();
        Vector3 lineP2 = lineP1 + imagePlane1.getImageOrientation().getNormalVector();
        Vector3 planeNormal = imagePlane2.getImageOrientation().getNormalVector();
        Vector3 planeP0 = imagePlane2.getCenter();

        if (MathTools::almostEqual(lineP1, planeP0, std::numeric_limits<double>::epsilon(), 1e-7))
        {
            DEBUG_LOG("Two planes with same center, probably rotational");
            return false;
        }

        double t;
        double x[3];
        int intersect = vtkPlane::IntersectWithLine(lineP1.toArray().data(), lineP2.toArray().data(), planeNormal.toArray().data(), planeP0.toArray().data(), t,
                                                    x);

        if (intersect == 0 && t == VTK_DOUBLE_MAX)
        {
            DEBUG_LOG("Spatially inconsistent because line and plane are parallel");
            return false;
        }
        else
        {
            // Check if intersection is inside imagePlane2
            Vector3 intersection(x);
            Vector3 projectedIntersection = imagePlane2.projectPoint(intersection);

            if (projectedIntersection.x < 0 || projectedIntersection.x > imagePlane2.getRowLength() ||
                    projectedIntersection.y < 0 || projectedIntersection.y > imagePlane2.getColumnLength())
            {
                DEBUG_LOG("Spatially inconsistent because intersection is out of rectangle");
                return false;
            }
        }
    }

    DEBUG_LOG("Spatially consistent");
    return true;
}

bool OrderImagesFillerStep::lesserSpatialPosition(const Image *image1, const Image *image2)
{
    double distance1 = Image::distance(image1);
    double distance2 = Image::distance(image2);

    if (distance1 != distance2)
    {
        return distance1 < distance2;
    }
    else
    {
        return lesserAbstractValues(image1, image2);
    }
}

bool OrderImagesFillerStep::lesserAbstractValues(const Image *image1, const Image *image2)
{
    auto uidToUintVector = [](const QString &uid) {
        // This is necessary because splitRef on an empty string returns a vector with an empty string
        if (uid.isEmpty())
        {
            return QVector<uint>();
        }

        QVector<QStringRef> strings = uid.splitRef('.');
        QVector<uint> values(strings.size());

        for (int i = 0; i < strings.size(); i++)
        {
            values[i] = strings[i].toUInt();
        }

        return values;
    };

    auto compareUintVector = [](const QVector<uint> &vector1, const QVector<uint> &vector2) {
        int size = std::min(vector1.size(), vector2.size());

        for (int i = 0; i < size; i++)
        {
            if (vector1[i] < vector2[i])
            {
                return -1;
            }
            else if (vector1[i] > vector2[i])
            {
                return 1;
            }
        }

        return vector1.size() - vector2.size();
    };

    int dimensionIndexValuesComparison = compareUintVector(image1->getDimensionIndexValues(), image2->getDimensionIndexValues());

    if (dimensionIndexValuesComparison != 0)
    {
        return dimensionIndexValuesComparison < 0;
    }

    if (image1->getInstanceNumber().toInt() != image2->getInstanceNumber().toInt())
    {
        return image1->getInstanceNumber().toInt() < image2->getInstanceNumber().toInt();
    }

    int sopInstanceUidComparison = compareUintVector(uidToUintVector(image1->getSOPInstanceUID()), uidToUintVector(image2->getSOPInstanceUID()));

    if (sopInstanceUidComparison != 0)
    {
        return sopInstanceUidComparison < 0;
    }

    return image1->getFrameNumber() < image2->getFrameNumber();
}

void OrderImagesFillerStep::autodetectStacksAndSort(QList<Image*> &images)
{
    // We need at least 2 stacks of 2 images to consider stacks
    if (images.size() < 4)
    {
        return;
    }

    QHash<ImageOrientation, int> orientationToStack;
    std::multimap<int, Image*> stackToImages;
    int nextStackNumber = 0;

    foreach (Image *image, images)
    {
        const ImageOrientation &orientation = image->getImageOrientationPatient();

        int stackNumber;
        bool detectedNewStack = !orientationToStack.contains(orientation);

        if (!detectedNewStack)  // existing stack
        {
            stackNumber = orientationToStack[orientation];

            if (stackToImages.count(stackNumber) >= 2)  // check if distance between slices is maintained
            {
                auto it = stackToImages.upper_bound(stackNumber);
                --it;
                Image *image2 = it->second;
                --it;
                Image *image1 = it->second;

                double d1 = Image::distance(image1);
                double d2 = Image::distance(image2);
                double d3 = Image::distance(image);
                double previousDistance = d2 - d1;
                double currentDistance = d3 - d2;

                // Since images are pre-sorted by distance with this order of subtraction both values will be positive
                // Warning: this will fail if there are unidentified stacks combined with phases, but we have never seen any such series yet
                detectedNewStack = !MathTools::almostEqual(previousDistance, currentDistance, MathTools::Epsilon, 1e-5); // different distance -> new stack
            }
        }

        if (detectedNewStack)
        {
            stackNumber = nextStackNumber;
            nextStackNumber++;
            orientationToStack[orientation] = stackNumber;
            // If there are several stacks with the same orientation we only need to keep the last one because they are pre-sorted
        }

        stackToImages.insert({stackNumber, image});
    }

    Q_ASSERT(images.size() == stackToImages.size());

    std::transform(stackToImages.begin(), stackToImages.end(), images.begin(), [](std::pair<int, Image*> pair) -> Image* { return pair.second; });
}

void OrderImagesFillerStep::spatialSort(QList<Image*> &images, bool severalOrientations)
{
    // Used to use the first image of each group as map key
    struct ImageWrapper
    {
        Image *image;
        ImageWrapper(Image *image = nullptr) : image(image) {}
        bool operator <(const ImageWrapper &that) const
        {
            return lesserSpatialPosition(this->image, that.image);
        }
    };

    QHash<int, QHash<QString, QList<Image*>>> imagesPerStackAndAcquisition;

    while (!images.isEmpty())
    {
        Image *image = images.takeFirst();
        int acquisitionNumber = image->getAcquisitionNumber().toInt();
        const QString &stackId = image->getStackId();
        // The list of images is created on the first access for each acquisition number and stack id
        imagesPerStackAndAcquisition[acquisitionNumber][stackId].append(image);
    }

    QMap<ImageWrapper, int> sortedAcquisitions;
    QHash<int, QMap<ImageWrapper, QString>> sortedStacksPerAcquisition;

    foreach (int acquisitionNumber, imagesPerStackAndAcquisition.keys())
    {
        foreach (const QString &stackId, imagesPerStackAndAcquisition[acquisitionNumber].keys())
        {
            QList<Image*> &localImages = imagesPerStackAndAcquisition[acquisitionNumber][stackId];
            std::sort(localImages.begin(), localImages.end(), lesserSpatialPosition);
            sortedStacksPerAcquisition[acquisitionNumber].insert(localImages.first(), stackId);

            if (severalOrientations && stackId.isEmpty())
            {
                autodetectStacksAndSort(localImages);
            }
        }

        sortedAcquisitions.insert(sortedStacksPerAcquisition[acquisitionNumber].firstKey(), acquisitionNumber);
    }

    foreach (int acquisitionNumber, sortedAcquisitions.values())
    {
        foreach (const QString &stackId, sortedStacksPerAcquisition[acquisitionNumber].values())
        {
            images.append(imagesPerStackAndAcquisition[acquisitionNumber][stackId]);
        }
    }
}

void OrderImagesFillerStep::basicSort(QList<Image*> &images)
{
    std::sort(images.begin(), images.end(), lesserAbstractValues);
}

QString OrderImagesFillerStep::SampleImagePerPosition::hashKey(const Image *image)
{
    constexpr int Width = 0;
    constexpr char Format = 'f';
    constexpr int Precision = 9;

    Vector3 position(image->getImagePositionPatient());
    ImageOrientation orientation = image->getImageOrientationPatient();
    QVector3D rowVector = orientation.getRowVector();
    QVector3D columnVector = orientation.getColumnVector();

    return QString("%1\\%2\\%3\\%4\\%5\\%6\\%7\\%8\\%9")
            .arg(    position.x,   Width, Format, Precision).arg(    position.y,   Width, Format, Precision).arg(    position.z,   Width, Format, Precision)
            .arg(   rowVector.x(), Width, Format, Precision).arg(   rowVector.y(), Width, Format, Precision).arg(   rowVector.z(), Width, Format, Precision)
            .arg(columnVector.x(), Width, Format, Precision).arg(columnVector.y(), Width, Format, Precision).arg(columnVector.z(), Width, Format, Precision);
}

void OrderImagesFillerStep::SampleImagePerPosition::add(Image *image)
{
    QString key = hashKey(image);

    if (!this->contains(key))
    {
        this->insert(key, image);
    }
}

}
