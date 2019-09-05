/*@
    "name": "test_OrderImagesFillerStep",
    "requirements": ["archive.order_images"]
 */

#include "autotest.h"
#include "orderimagesfillerstep.h"

#include "image.h"
#include "patient.h"
#include "patientfillerinput.h"

using namespace udg;

namespace {

class ImageBuilder
{
public:
    ImageBuilder()
    {
        static int uid = 0;
        m_image = new Image();
        m_image->setSOPInstanceUID(QString::number(uid++));
        m_image->setImageOrientationPatient(ImageOrientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)));
        m_image->setVolumeNumberInSeries(1);
        m_image->setPixelSpacing(1, 1);
        m_image->setColumns(256);
        m_image->setRows(256);
    }

    ImageBuilder& uid(int uid)
    {
        m_image->setSOPInstanceUID(QString::number(uid));
        return *this;
    }

    ImageBuilder& instance(int instanceNumber)
    {
        m_image->setInstanceNumber(QString::number(instanceNumber));
        return *this;
    }

    ImageBuilder& frame(int frameNumber)
    {
        m_image->setFrameNumber(frameNumber);
        return *this;
    }

    ImageBuilder& acquisition(int acquisitionNumber)
    {
        m_image->setAcquisitionNumber(QString::number(acquisitionNumber));
        return *this;
    }

    ImageBuilder& position(const Vector3 &position)
    {
        m_image->setImagePositionPatient(position.toArray().data());
        return *this;
    }

    ImageBuilder& orientation(const QVector3D &rowVector, const QVector3D &columnVector)
    {
        m_image->setImageOrientationPatient(ImageOrientation(rowVector, columnVector));
        return *this;
    }

    ImageBuilder& spacing(double x, double y)
    {
        m_image->setPixelSpacing(x, y);
        return *this;
    }

    ImageBuilder& size(int rows, int columns)
    {
        m_image->setRows(rows);
        m_image->setColumns(columns);
        return *this;
    }

    ImageBuilder& volume(int volumeNumber)
    {
        m_image->setVolumeNumberInSeries(volumeNumber);
        return *this;
    }

    Image* get() const
    {
        return m_image;
    }

private:
    Image *m_image;
};

// Creates a list of lists of 1 image with the given images.
QList<QList<Image*>> imagesOneByOne(const QList<QList<Image*>> &allImages)
{
    QList<QList<Image*>> result;

    foreach (const QList<Image*> &images, allImages)
    {
        foreach (Image *image, images)
        {
            result.append({image});
        }
    }

    return result;
}

Series* shuffleAndCreateSeries(QList<QList<Image*>> &arrivingImages)
{
    std::random_shuffle(arrivingImages.begin(), arrivingImages.end());

    Series *series = new Series();

    foreach (const QList<Image*> &images, arrivingImages)
    {
        foreach (Image *image, images)
        {
            series->addImage(image);
        }
    }

    return series;
}

}

class test_OrderImagesFillerStep : public QObject {

    Q_OBJECT

private slots:

    // Actually these test the whole class
    void postProcessing_ShouldOrderImagesAsExpected_data();
    void postProcessing_ShouldOrderImagesAsExpected();

    void canBeSpatiallySorted_ShouldReturnExpectedValue_data();
    void canBeSpatiallySorted_ShouldReturnExpectedValue();

private:

    void setupData();

};

void test_OrderImagesFillerStep::postProcessing_ShouldOrderImagesAsExpected_data()
{
    setupData();
}

void test_OrderImagesFillerStep::postProcessing_ShouldOrderImagesAsExpected()
{
    QFETCH(QList<QList<Image*>>, arrivingImages);
    QFETCH(QList<Series*>, seriesList);
    QFETCH(QList<QList<Image*>>, sortedImages);
    QFETCH(QList<QList<int>>, orderNumbers);

    PatientFillerInput *input = new PatientFillerInput();
    OrderImagesFillerStep orderImages;
    orderImages.setInput(input);

    foreach (const QList<Image*> &images, arrivingImages)
    {
        input->setCurrentSeries(images.first()->getParentSeries());
        input->setCurrentVolumeNumber(images.first()->getVolumeNumberInSeries());
        input->setCurrentImages(images, false);

        QVERIFY(orderImages.fillIndividually());
    }

    orderImages.postProcessing();

    delete input;

    for (int i = 0; i < seriesList.size(); i++)
    {
        QCOMPARE(seriesList[i]->getImages(), sortedImages[i]);

        for (int j = 0; j < sortedImages[i].size(); j++)
        {
            QCOMPARE(sortedImages[i][j]->getOrderNumberInVolume(), orderNumbers[i][j]);
        }

        delete seriesList[i];
    }
}

void test_OrderImagesFillerStep::canBeSpatiallySorted_ShouldReturnExpectedValue_data()
{
    setupData();
}

void test_OrderImagesFillerStep::canBeSpatiallySorted_ShouldReturnExpectedValue()
{
    QFETCH(QList<QList<Image*>>, arrivingImages);
    QFETCH(QList<Series*>, seriesList);
    QFETCH(QList<QList<bool>>, canBeSpatiallySorted);

    PatientFillerInput *input = new PatientFillerInput();
    OrderImagesFillerStep orderImages;
    orderImages.setInput(input);

    foreach (const QList<Image*> &images, arrivingImages)
    {
        input->setCurrentSeries(images.first()->getParentSeries());
        input->setCurrentVolumeNumber(images.first()->getVolumeNumberInSeries());
        input->setCurrentImages(images, false);

        QVERIFY(orderImages.fillIndividually());
    }

    orderImages.postProcessing();

    delete input;

    for (int i = 0; i < seriesList.size(); i++)
    {
        for (int j = 1; j <= canBeSpatiallySorted[i].size(); j++)
        {
            QCOMPARE(orderImages.canBeSpatiallySorted(seriesList[i], j), canBeSpatiallySorted[i][j-1]);
        }

        delete seriesList[i];
    }
}

void test_OrderImagesFillerStep::setupData()
{
    QTest::addColumn<QList<QList<Image*>>>("arrivingImages");       // for each processed file (outer list), list of generated images (inner list)
    QTest::addColumn<QList<Series*>>("seriesList");
    QTest::addColumn<QList<QList<Image*>>>("sortedImages");         // for each series (outer list), list of sorted images (inner list)
    QTest::addColumn<QList<QList<int>>>("orderNumbers");            // for each series (outer list) and image in it (inner list), order number in volume
    QTest::addColumn<QList<QList<bool>>>("canBeSpatiallySorted");   // for each series (outer list) and volume (inner), expected result of canBeSpatiallySorted

    // Note: image attributes are set in a clever way so that only the expected sorting criteria produce the correct order, thus if images are accidentally
    // sorted with different criteria they will be in a completely different and wrong order.
    // E.g. if it must be sorted by 'distance' then instance numbers will be set so that they produce a different order.

    {
        Series *series = new Series();
        Image *image = ImageBuilder().get();
        series->addImage(image);

        QList<QList<Image*>> arrivingImages{{image}};
        QList<Series*> seriesList{series};
        QList<QList<Image*>> sortedImages{{image}};
        QList<QList<int>> orderNumbers{{0}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 0: 1 image") << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> arrivingImages;
        QList<QList<Image*>> sortedImages{{}};

        for (int i = 0; i < 4; i++)
        {

            Image *image = ImageBuilder().position(Vector3(0, 0, i)).get();
            arrivingImages.append({image});
            sortedImages[0].append(image);
        }

        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 1: n images, nothing strange -> order by 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> arrivingImages;
        QList<QList<Image*>> sortedImages{{}};

        for (int i = 0, uid = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++, uid++)
            {
                Image *image = ImageBuilder().position(Vector3(0, 0, i)).instance(j).get();
                arrivingImages.append({image});
                sortedImages[0].append(image);
            }
        }

        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 2: 3 positions, 3 phases -> order by 'distance' and instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(1, 0, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(1, 0, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(4).position(Vector3(0, 0, 1)).get(),
                                           ImageBuilder().instance(5).position(Vector3(0, 0, -1)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 3: irregular phases -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(5).position(Vector3(0, 0, -1)).get(),
                                           ImageBuilder().instance(0).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(1, 0, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(4).position(Vector3(0, 0, 1)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 4: [bug or feature?] irregular phases only in position (0,0,0) "
                      "-> order by minimum 'distance' in the normal, 'distance' and instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).acquisition(1).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(1).acquisition(1).position(Vector3(0, 0, 1)).get(),
                                           ImageBuilder().instance(2).acquisition(2).position(Vector3(0, 0, 4)).get(),
                                           ImageBuilder().instance(3).acquisition(2).position(Vector3(0, 0, 2)).get(),
                                           ImageBuilder().instance(4).acquisition(2).position(Vector3(0, 0, 3)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 5: multiple acquisitions -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().position(Vector3(0, 0, 0)).instance(4).get(),
                                           ImageBuilder().position(Vector3(0, 0, 1)).instance(12).get(),
                                           ImageBuilder().position(Vector3(0, 0, 2)).instance(1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 10)).instance(1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 11)).instance(0).get(),
                                           ImageBuilder().position(Vector3(0, 0, 12)).instance(8).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 6: aligned stacks with the same normal -> order by 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().position(Vector3(10, 0, 0)).instance(1).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 0)).instance(4).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(10, 0, 1)).instance(0).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 1)).instance(12).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 2)).instance(5).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(10, 0, 2)).instance(8).size(10, 10).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 7: parallel stacks with the same normal -> order by 'distance' and instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().position(Vector3(0, 0, 0)).instance(1).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 1)).instance(4).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 2)).instance(0).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 3)).instance(9).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 4)).instance(5).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 5)).instance(8).size(1, 1).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 8: zig-zag stacks with the same normal -> order by 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(1).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(7).position(Vector3(0, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(3).position(Vector3(0, 0, -4)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(5).position(Vector3(0, 0, 1.5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get(),
                ImageBuilder().instance(8).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        // Put 1st and 3rd images at the beginning and keep them in order because they define angle 0 and rotation direction
        arrivingImages.swap(1, 2);
        std::random_shuffle(arrivingImages.begin() + 2, arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 9: small stacks (distance <= 1) with different normals (varying progressively) (detected as rotationals) "
                      "-> order by 'angle', minimum 'distance' in the normal and 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(1).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(7).position(Vector3(0, 0, 5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(3).position(Vector3(0, 0, 7)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(5).position(Vector3(0, 0, 10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get(),
                ImageBuilder().instance(8).position(Vector3(0, 0, 12)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        std::random_shuffle(arrivingImages.begin(), arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 10: big stacks (distance > 1) with different normals (varying progressively) "
                      "-> order by minimum 'distance' in the normal and 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(3).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(6).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(0).position(Vector3(0, 0, 10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.95630476f, -0.29237170f)).get(),
                ImageBuilder().instance(7).position(Vector3(0, 0, 12)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.95630476f, -0.29237170f)).get(),
                ImageBuilder().instance(8).position(Vector3(0, 20, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, -0.20791169f, -0.97814760f)).get(),
                ImageBuilder().instance(1).position(Vector3(0, 23, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, -0.20791169f, -0.97814760f)).get(),
                ImageBuilder().instance(4).position(Vector3(0, 0, -30)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get(),
                ImageBuilder().instance(2).position(Vector3(0, 0, -32)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get(),
                ImageBuilder().instance(5).position(Vector3(0, 0, -36)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        std::random_shuffle(arrivingImages.begin(), arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 11: big stacks with different normals (varying randomly) -> order by minimum 'distance' in the normal and 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // 9 positions starting from angle 0 in increments of 40º
        // Positions generated with x0 = -5, z0 = -5, angleInRadians = angleInDegrees * PI / 180,
        //                          x = x0 * cos(angleInRadians) - z0 * sin(angleInRadians),
        //                          y = 0,
        //                          z = x0 * sin(angleInRadians) + z0 * cos(angleInRadians)
        // Row vectors with the same formula with x0 = 1, z0 = 0
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(7).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(2).position(Vector3(-0.61628417, 0, -7.04416026))
                              .orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(9).position(Vector3(4.05579788, 0, -5.79227965))
                              .orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(5).position(Vector3(6.83012702, 0, -1.83012702))
                              .orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(6.40856382, 0, 2.98836239))
                              .orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(3).position(Vector3(2.98836239, 0, 6.40856382))
                              .orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(8).position(Vector3(-1.83012702, 0, 6.83012702))
                              .orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(6).position(Vector3(-5.79227965, 0, 4.05579788))
                              .orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(1).position(Vector3(-7.04416026, 0, -0.61628417))
                              .orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);

        // Keep first 2 images in order because they define angle 0 and rotation direction
        std::random_shuffle(arrivingImages.begin() + 2, arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 12: rotational -> order by 'angle'") << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().uid(0).frame(1).position(Vector3(10, 0, 0)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(2).position(Vector3(0, 0, 0)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(3).position(Vector3(0, 0, 1)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(4).position(Vector3(10, 0, 1)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(0).position(Vector3(10, 0, 2)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(5).position(Vector3(0, 0, 2)).size(10, 10).get()}};
        QList<QList<Image*>> arrivingImages(sortedImages);
        std::random_shuffle(arrivingImages[0].begin(), arrivingImages[0].end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 13: multiframe volume with parallel stacks with the same normal -> order by 'distance' and instance number and frame number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<Image*> images{ImageBuilder().uid(0).instance(4).frame(0).position(Vector3(0, 0, 0)).volume(1).get(),
                             ImageBuilder().uid(1).instance(12).frame(0).position(Vector3(0, 0, 1)).volume(1).get(),
                             ImageBuilder().uid(6).instance(5).frame(0).position(Vector3(0, 0, 2)).volume(1).get(),
                             ImageBuilder().uid(3).instance(2).frame(0).position(Vector3(0, 0, 10)).volume(1).get(),
                             ImageBuilder().uid(5).instance(0).frame(0).position(Vector3(0, 0, 11)).volume(1).get(),
                             ImageBuilder().uid(4).instance(8).frame(0).position(Vector3(0, 0, 12)).volume(1).get(),
                             ImageBuilder().uid(10).instance(1).frame(0).position(Vector3(0, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(1).position(Vector3(1, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(2).position(Vector3(1, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(3).position(Vector3(0, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(4).position(Vector3(0, 0, 1)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(5).position(Vector3(0, 0, -1)).volume(2).get()};
        QList<QList<Image*>> sortedImages{images};
        QList<QList<Image*>> arrivingImages;
        auto it = images.begin();

        for (int i = 0; i < 6; i++, ++it)
        {
            arrivingImages.append({*it});
        }

        QList<Image*> multiframeImages;
        std::copy(it, images.end(), std::back_inserter(multiframeImages));
        std::random_shuffle(multiframeImages.begin(), multiframeImages.end());
        arrivingImages.append(multiframeImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true, false}};

        QTest::newRow("Case 14: multi-volume, each volume with different criteria")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // One series with 2 volumes and another with 1 volume, all multi-frame
        QList<Image*> file1{
            ImageBuilder().uid(1).instance(1).frame(2).size(10, 10).position(Vector3(10, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(3).size(10, 10).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(1).size(10, 10).position(Vector3(10, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(4).size(10, 10).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(0).size(10, 10).position(Vector3(10, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(5).size(10, 10).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get()
        };
        QList<Image*> file2{
            ImageBuilder().uid(2).instance(2).frame(0).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(2).position(Vector3(-0.61628417, 0, -7.04416026))
                          .orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(4).position(Vector3(4.05579788, 0, -5.79227965))
                          .orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(3).position(Vector3(6.83012702, 0, -1.83012702))
                          .orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(7).position(Vector3(6.40856382, 0, 2.98836239))
                          .orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(8).position(Vector3(2.98836239, 0, 6.40856382))
                          .orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(6).position(Vector3(-1.83012702, 0, 6.83012702))
                          .orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(1).position(Vector3(-5.79227965, 0, 4.05579788))
                          .orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(5).position(Vector3(-7.04416026, 0, -0.61628417))
                          .orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0)).volume(1).get()
        };
        QList<Image*> file3;

        for (int i = 0; i < 4; i++)
        {
            Image *image = ImageBuilder().uid(3).instance(3).frame(i).position(Vector3(0, 0, i)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(2)
                                         .get();
            file3.append(image);
        }

        QList<QList<Image*>> sortedImages{{file1 + file3}, {file2}};
        std::random_shuffle(file1.begin(), file1.end());
        std::random_shuffle(file2.begin() + 2, file2.end());    // keep first 2 images in order because they define angle 0 and rotation direction
        std::random_shuffle(file3.begin(), file3.end());
        QList<QList<Image*>> arrivingImages{file3, file1, file2};
        Series *series1 = new Series();

        foreach (Image *image, file3 + file1)
        {
            series1->addImage(image);
        }

        Series *series2 = new Series();

        foreach (Image *image, file2)
        {
            series2->addImage(image);
        }

        QList<Series*> seriesList{series1, series2};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 0, 1, 2, 3}, {0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false, true}, {false}};

        QTest::newRow("Case 15: multi-series and multi-volume, each case with different criteria")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Note: created with unrealistic properties (single frame images, non-consecutive frame numbers, etc.) because it's shorter
        QList<QList<Image*>> sortedImages{{ImageBuilder().uid(1).instance(1).frame(0).acquisition(1).position(Vector3(0, 0, 6)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(5).acquisition(1).position(Vector3(0, 0, 1)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(6).acquisition(2).position(Vector3(0, 0, -4)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(9).acquisition(2).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(4).acquisition(3).position(Vector3(0, 0, 8)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(6).acquisition(3).position(Vector3(0, 0, -5)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(10).acquisition(2).position(Vector3(0, 0, 7)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(20).acquisition(2).position(Vector3(0, 0, 2)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(44).acquisition(3).position(Vector3(0, 0, -1)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(64).acquisition(3).position(Vector3(0, 0, -2)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(100).acquisition(3).position(Vector3(0, 0, 10)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 16: [bug] different instance numbers, frame numbers and acquisitions -> order by instance number and frame number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
        // Note that this is an unrealistic case, it can't happen at this moment because VolumeFillerStep would create separate volumes, but better to have it
        // covered just in case it becomes possible in the future
    }

    {
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().instance(8).position(Vector3(0, 12, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(0).position(Vector3(0, 11, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(1).position(Vector3(0, 10, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(5).position(Vector3(0, 2, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(9).position(Vector3(0, 1, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(4).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(17).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(12).position(Vector3(-0.61628417, 0, -7.04416026)).orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(19).position(Vector3(4.05579788, 0, -5.79227965)).orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(15).position(Vector3(6.83012702, 0, -1.83012702)).orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(14).position(Vector3(6.40856382, 0, 2.98836239)).orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(13).position(Vector3(2.98836239, 0, 6.40856382)).orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(18).position(Vector3(-1.83012702, 0, 6.83012702)).orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(16).position(Vector3(-5.79227965, 0, 4.05579788)).orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(11).position(Vector3(-7.04416026, 0, -0.61628417)).orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0))
                          .get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        // Put the first 2 rotational images at the beginning and keep them in order because they define angle 0 and rotation direction
        arrivingImages.swap(0, 6);
        arrivingImages.swap(1, 7);
        std::random_shuffle(arrivingImages.begin() + 2, arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 17: stacks and rotational in the same series (stacks with a normal not contained in the rotational) "
                      "-> stacks come first, then rotationals")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().instance(17).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(4).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(9).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(5).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(1).position(Vector3(0, 0, 10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(0).position(Vector3(0, 0, 11)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(8).position(Vector3(0, 0, 12)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(12).position(Vector3(-0.61628417, 0, -7.04416026)).orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(19).position(Vector3(4.05579788, 0, -5.79227965)).orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(15).position(Vector3(6.83012702, 0, -1.83012702)).orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(14).position(Vector3(6.40856382, 0, 2.98836239)).orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(13).position(Vector3(2.98836239, 0, 6.40856382)).orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(18).position(Vector3(-1.83012702, 0, 6.83012702)).orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(16).position(Vector3(-5.79227965, 0, 4.05579788)).orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(11).position(Vector3(-7.04416026, 0, -0.61628417)).orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0))
                          .get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        // Put the first 2 rotational images at the beginning and keep them in order because they define angle 0 and rotation direction
        arrivingImages.swap(1, 7);
        std::random_shuffle(arrivingImages.begin() + 2, arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 18: stacks and rotational in the same series (stacks with a normal contained in the rotational) -> "
                      "stacks come first, then rotationals (the rotational image with the same normal as the stacks is mixed with them)")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{}};

        for (int j = 0, uid = 0; j < 3; j++, uid++)
        {
            Image *image = ImageBuilder().frame(j).get();
            sortedImages[0].append(image);
        }

        QList<QList<Image*>> arrivingImages(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 19: 1 position, 3 phases -> order by instance number and frame number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // This is copied from a real CT localizer
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(1).acquisition(1).position(Vector3(0, -175.7, 150.01875)).orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).get(),
                ImageBuilder().instance(1).acquisition(0).position(Vector3(-250, 74.3, 150.01875)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, -1)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        std::reverse(arrivingImages.begin(), arrivingImages.end());
        Series *series = new Series();
        series->addImage(arrivingImages[0][0]);
        series->addImage(arrivingImages[1][0]);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 20: 2 images with same instance number and different acquisition number and orientation -> order by reverse arrival order")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of first series of study from #351 comment 1 (DimensionIndexValues available)
        QList<QList<Image*>> arrivingImages{{
                ImageBuilder().uid(0).instance(1).frame(0).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(11, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).get(),
                ImageBuilder().uid(0).instance(1).frame(1).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(0, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).get(),
                ImageBuilder().uid(0).instance(1).frame(2).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-11, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).get(),
                ImageBuilder().uid(0).instance(1).frame(3).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, -16.880112528800, 145.811029553413))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).get(),
                ImageBuilder().uid(0).instance(1).frame(4).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, -6.0472267866134, 147.721159458160))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).get(),
                ImageBuilder().uid(0).instance(1).frame(5).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, 4.78565895557403, 149.631289362907))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).get(),
                ImageBuilder().uid(0).instance(1).frame(6).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, -20))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().uid(0).instance(1).frame(7).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, 0))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().uid(0).instance(1).frame(8).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, 20))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get()
        }};
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<Image*>> sortedImages{{arrivingImages[0][6], arrivingImages[0][7], arrivingImages[0][8],
                                           arrivingImages[0][0], arrivingImages[0][1], arrivingImages[0][2],
                                           arrivingImages[0][3], arrivingImages[0][4], arrivingImages[0][5]}};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 21: different orientations not rotational, detected as stacks -> order by minimum 'distance' in the normal and 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of series 501 of study from #477 (DimensionIndexValues available)
        QList<QList<Image*>> arrivingImages{{
                ImageBuilder().uid(0).instance(1).frame(0).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(18.9671724950894, -179.84025764465, -592.58819651603))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(1).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(10.1672152196988, -179.84025764465, -592.56066966056))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(2).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(1.36725794430822, -179.84025764465, -592.53308176994))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(3).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-7.4326988542452, -179.84025764465, -592.50549387931))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(4).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-16.232656606473, -179.84025764465, -592.47796702384))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(5).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(19.9070910131558, -179.84025764465, -292.58969187736))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(6).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(11.1071327840909, -179.84025764465, -292.56210398674))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(7).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(2.30717622395604, -179.84025764465, -292.53454661369))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(8).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-6.4927808130159, -179.84025764465, -292.50698924064))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(9).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-15.292738088406, -179.84025764465, -292.47940135002))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(10).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(20.8470076238736, -179.84025764465, 7.40884327888488))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(11).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(12.0470513021573, -179.84025764465, 7.43641591072082))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(12).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(3.24709450360387, -179.84025764465, 7.46398854255676))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(13).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-5.5528622949495, -179.84025764465, 7.4915611743927))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get(),
                ImageBuilder().uid(0).instance(1).frame(14).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-14.352819570340, -179.84025764465, 7.51913380622863))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).get()
        }};
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<Image*>> sortedImages{{arrivingImages[0][5], arrivingImages[0][0], arrivingImages[0][10], arrivingImages[0][1], arrivingImages[0][11],
                                           arrivingImages[0][6], arrivingImages[0][12], arrivingImages[0][7], arrivingImages[0][2], arrivingImages[0][13],
                                           arrivingImages[0][3], arrivingImages[0][8], arrivingImages[0][14], arrivingImages[0][9], arrivingImages[0][4]}};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 22: 3 parallel stacks -> order by minimum 'distance' in the normal and 'distance' (affected by numerical precision)")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 1)).orientation(QVector3D(0, 1, 0), QVector3D(-1, 0, 0)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 23: 1 position with 2 phases, another position with 2 orientations but same normal "
                      "-> detected as 2 regular phases, order by position and instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        // Preserve the first image at the beginning because it defines angle 0
        std::random_shuffle(arrivingImages.begin() + 1, arrivingImages.end());
        Series *series = new Series();

        foreach (const QList<Image*> &images, arrivingImages)
        {
            foreach (Image *image, images)
            {
                series->addImage(image);
            }
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 24: 1 position with 2 phases, another position with 2 orientations and different normal "
                      "-> detected as 2 regular phases, order by position and instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of a subset of series 10 of study from #682 comment 1
        // Note that the z component of the orientation row vector changes in the 8th decimal position at some point
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(20).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-217.215, -85.0388, 88.4411))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(19).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-217.171, -85.6781, 95.9136))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(18).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-217.127, -86.3174, 103.386))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(17).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-217.083, -86.9568, 110.859))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(16).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-217.039, -87.5961, 118.331))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(15).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.995, -88.2354, 125.804))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592066f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(14).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.951, -88.8747, 133.277))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(13).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.907, -89.514, 140.749))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(12).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.863, -90.1533, 148.222))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(11).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.819, -90.7927, 155.694))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(10).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.775, -91.432, 163.167))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(9).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.732, -92.0713, 170.639))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(8).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.688, -92.7106, 178.112))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get(),
                ImageBuilder().instance(7).size(512, 512).spacing(0.4297, 0.4297).position(Vector3(-216.644, -93.3499, 185.584))
                              .orientation(QVector3D(0.999982f, -0.000488288f, -0.00592069f), QVector3D(0.00100711f, 0.99636f, 0.0852375f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 25: very slight variation in orientation -> order by 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }
}

DECLARE_TEST(test_OrderImagesFillerStep)

#include "test_orderimagesfillerstep.moc"
