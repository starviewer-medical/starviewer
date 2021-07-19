/*@
    "name": "test_OrderImagesFillerStep",
    "requirements": ["archive.image_order"]
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

    ImageBuilder& uid(const QString &uid)
    {
        m_image->setSOPInstanceUID(uid);
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

    ImageBuilder& stackId(QString stackId)
    {
        m_image->setStackId(std::move(stackId));
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

    ImageBuilder& dimensionIndexValues(QVector<uint> dimensionIndexValues)
    {
        m_image->setDimensionIndexValues(std::move(dimensionIndexValues));
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

    // Actually these test the whole sorting mechanism
    void postProcessing_ShouldOrderImagesAsExpected_data();
    void postProcessing_ShouldOrderImagesAsExpected();

    void canBeSpatiallySorted_ShouldReturnExpectedValue_data();
    void canBeSpatiallySorted_ShouldReturnExpectedValue();

    void lesserSpatialPosition_ShouldReturnExpectedValue_data();
    void lesserSpatialPosition_ShouldReturnExpectedValue();

    void lesserAbstractValues_ShouldReturnExpectedValue_data();
    void lesserAbstractValues_ShouldReturnExpectedValue();

    void autodetectStacksAndSort_ShouldUpdateOrderAsExpected_data();
    void autodetectStacksAndSort_ShouldUpdateOrderAsExpected();

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

void test_OrderImagesFillerStep::lesserSpatialPosition_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Image*>("image1");
    QTest::addColumn<Image*>("image2");
    QTest::addColumn<bool>("expectedValue");

    Image *image1 = new Image();
    Image *image2 = new Image();
    QTest::newRow("equal images") << image1 << image2 << false;

    image1 = ImageBuilder().position(Vector3(10, 9, 23)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get();
    image2 = ImageBuilder().position(Vector3(10, 9, 80)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get();
    QTest::newRow("lesser distance") << image1 << image2 << true;

    image1 = ImageBuilder().position(Vector3(0, -3, 13)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get();
    image2 = ImageBuilder().position(Vector3(83, 12, -10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get();
    QTest::newRow("greater distance") << image1 << image2 << false;

    image1 = ImageBuilder().position(Vector3(1, -3, 15)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).instance(1).get();
    image2 = ImageBuilder().position(Vector3(8, 0, 15)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).instance(2).get();
    QTest::newRow("same distance, lesser instance number") << image1 << image2 << true;

    image1 = ImageBuilder().position(Vector3(1, -3, 15)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).instance(2).get();
    image2 = ImageBuilder().position(Vector3(8, 0, 15)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).instance(1).get();
    QTest::newRow("same distance, greater instance number") << image1 << image2 << false;
}

void test_OrderImagesFillerStep::lesserSpatialPosition_ShouldReturnExpectedValue()
{
    QFETCH(Image*, image1);
    QFETCH(Image*, image2);
    QFETCH(bool, expectedValue);

    QCOMPARE(OrderImagesFillerStep::lesserSpatialPosition(image1, image2), expectedValue);

    delete image1;
    delete image2;
}

void test_OrderImagesFillerStep::lesserAbstractValues_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Image*>("image1");
    QTest::addColumn<Image*>("image2");
    QTest::addColumn<bool>("expectedValue");

    Image *image1 = new Image();
    Image *image2 = new Image();
    QTest::newRow("equal images") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({2}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 1 value, lesser") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({3}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({2}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 1 value, greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 7}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({2, 4}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 2 values, decide by first, lesser") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({2, 6}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 8}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 2 values, decide by first, greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({3, 2}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({3, 5}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 2 values, decide by second, lesser") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({3, 3}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({3, 1}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 2 values, decide by second, greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 3, 3}).instance(0).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 1, 4}).instance(0).uid(0).frame(0).get();
    QTest::newRow("dimension index values: 3 values, decide by second, greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(2).uid(0).frame(0).get();
    QTest::newRow("instance number: lesser") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(5).uid(0).frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(2).uid(0).frame(0).get();
    QTest::newRow("instance number: greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.5.8.1846").frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.5.12.0").frame(0).get();
    QTest::newRow("uid: lesser at third value") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.5.8.1846").frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.12.0").frame(0).get();
    QTest::newRow("uid: greater at second value") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(0).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(1).get();
    QTest::newRow("frame number: lesser") << image1 << image2 << true;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(3).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(2).get();
    QTest::newRow("frame number: greater") << image1 << image2 << false;

    image1 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(4).get();
    image2 = ImageBuilder().dimensionIndexValues({1, 2, 3}).instance(1).uid("1.2.3.4").frame(4).get();
    QTest::newRow("everything filled and equal") << image1 << image2 << false;
}

void test_OrderImagesFillerStep::lesserAbstractValues_ShouldReturnExpectedValue()
{
    QFETCH(Image*, image1);
    QFETCH(Image*, image2);
    QFETCH(bool, expectedValue);

    QCOMPARE(OrderImagesFillerStep::lesserAbstractValues(image1, image2), expectedValue);

    delete image1;
    delete image2;
}

void test_OrderImagesFillerStep::autodetectStacksAndSort_ShouldUpdateOrderAsExpected_data()
{
    QTest::addColumn<QList<Image*>>("images");
    QTest::addColumn<QList<Image*>>("expectedOutputOrder");

    {
        QTest::newRow("empty list") << QList<Image*>{} << QList<Image*>{};
    }

    {
        QList<Image*> images{new Image(), new Image(), new Image()};
        QTest::newRow("less than 4 images") << images << images;
    }

    {
        QList<Image*> images{ImageBuilder().position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 3)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get()};
        QTest::newRow("one stack regular spacing (||||)") << images << images;
    }

    {
        QList<Image*> images{ImageBuilder().position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 3)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 4)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get()};
        QTest::newRow("one stack irregular spacing (|| ||)") << images << images;
    }

    {
        QList<Image*> images{ImageBuilder().position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 2)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                             ImageBuilder().position(Vector3(0, 0, 3)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get()};
        QTest::newRow("two separated stacks (||//)") << images << images;
    }

    {
        QList<Image*> images{ImageBuilder().position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 1)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                             ImageBuilder().position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 3)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get()};
        QList<Image*> expectedOutputOrder{images[0], images[2], images[1], images[3]};
        QTest::newRow("two intersecting stacks (|/|/)") << images << expectedOutputOrder;
    }

    {
        QList<Image*> images{ImageBuilder().position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 1)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                             ImageBuilder().position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 3)).orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                                                                   QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                             ImageBuilder().position(Vector3(0, 0, 5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                             ImageBuilder().position(Vector3(0, 0, 6)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get()};
        QList<Image*> expectedOutputOrder{images[0], images[2], images[1], images[3], images[4], images[5]};
        QTest::newRow("three stacks, two with same orientation (|/|/ ||)") << images << expectedOutputOrder;
    }
}

void test_OrderImagesFillerStep::autodetectStacksAndSort_ShouldUpdateOrderAsExpected()
{
    QFETCH(QList<Image*>, images);
    QFETCH(QList<Image*>, expectedOutputOrder);

    OrderImagesFillerStep::autodetectStacksAndSort(images);

    QCOMPARE(images, expectedOutputOrder);

    foreach (Image *image, images)
    {
        delete image;
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
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(5).position(Vector3(0, 0, -1)).get(),
                                           ImageBuilder().instance(0).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(1, 0, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(1, 0, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(4).position(Vector3(0, 0, 1)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 3: irregular phases -> order by 'distance' and instance number")
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
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 4: irregular phases only in position (0,0,0) -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).acquisition(1).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().instance(1).acquisition(1).position(Vector3(0, 0, 1)).get(),
                                           ImageBuilder().instance(3).acquisition(2).position(Vector3(0, 0, 2)).get(),
                                           ImageBuilder().instance(4).acquisition(2).position(Vector3(0, 0, 3)).get(),
                                           ImageBuilder().instance(2).acquisition(2).position(Vector3(0, 0, 4)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 5: multiple acquisitions -> order by 'distance'")
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
        QList<QList<Image*>> sortedImages{{ImageBuilder().position(Vector3(10, 0, 1)).instance(0).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(10, 0, 0)).instance(1).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 0)).instance(4).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 2)).instance(5).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(10, 0, 2)).instance(8).size(10, 10).get(),
                                           ImageBuilder().position(Vector3(0, 0, 1)).instance(12).size(10, 10).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 7: parallel stacks with the same normal -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().position(Vector3(0, 0, 2)).instance(0).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 0)).instance(1).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 1)).instance(4).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(0, 0, 4)).instance(5).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 5)).instance(8).size(1, 1).get(),
                                           ImageBuilder().position(Vector3(1, 0, 3)).instance(9).size(1, 1).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 8: zig-zag stacks with the same normal -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(7).position(Vector3(0, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(3).position(Vector3(0, 0, -4)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99939083f, 0.03489950f)).get(),
                ImageBuilder().instance(1).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(5).position(Vector3(0, 0, 1.5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get(),
                ImageBuilder().instance(8).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.99756405f, 0.06975647f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 9: small stacks (distance <= 1) with different normals (varying progressively) -> order by 'distance'")
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
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 10: big stacks (distance > 1) with different normals (varying progressively) -> order by 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(0).position(Vector3(0, 0, 10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.95630476f, -0.29237170f)).get(),
                ImageBuilder().instance(1).position(Vector3(0, 23, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, -0.20791169f, -0.97814760f)).get(),
                ImageBuilder().instance(2).position(Vector3(0, 0, -32)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get(),
                ImageBuilder().instance(3).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(0, 0, -30)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get(),
                ImageBuilder().instance(5).position(Vector3(0, 0, -36)).orientation(QVector3D(1, 0, 0), QVector3D(0, -1, 0)).get(),
                ImageBuilder().instance(6).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(7).position(Vector3(0, 0, 12)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0.95630476f, -0.29237170f)).get(),
                ImageBuilder().instance(8).position(Vector3(0, 20, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, -0.20791169f, -0.97814760f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 11: big stacks with different normals (varying randomly) -> order by instance number")
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
                ImageBuilder().instance(1).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(2).position(Vector3(-0.61628417, 0, -7.04416026))
                              .orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(3).position(Vector3(4.05579788, 0, -5.79227965))
                              .orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(4).position(Vector3(6.83012702, 0, -1.83012702))
                              .orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(5).position(Vector3(6.40856382, 0, 2.98836239))
                              .orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(6).position(Vector3(2.98836239, 0, 6.40856382))
                              .orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(7).position(Vector3(-1.83012702, 0, 6.83012702))
                              .orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(8).position(Vector3(-5.79227965, 0, 4.05579788))
                              .orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0)).get(),
                ImageBuilder().instance(9).position(Vector3(-7.04416026, 0, -0.61628417))
                              .orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 12: rotational -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().uid(0).frame(0).position(Vector3(10, 0, 2)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(1).position(Vector3(10, 0, 0)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(2).position(Vector3(0, 0, 0)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(3).position(Vector3(0, 0, 1)).size(10, 10).get(),
                                           ImageBuilder().uid(0).frame(4).position(Vector3(10, 0, 1)).size(10, 10).get(),
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

        QTest::newRow("Case 13: multiframe volume with parallel stacks with the same normal -> order by frame number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<Image*> images{ImageBuilder().uid(0).instance(4).frame(0).position(Vector3(0, 0, 0)).volume(1).get(),
                             ImageBuilder().uid(1).instance(12).frame(0).position(Vector3(0, 0, 1)).volume(1).get(),
                             ImageBuilder().uid(6).instance(5).frame(0).position(Vector3(0, 0, 2)).volume(1).get(),
                             ImageBuilder().uid(3).instance(2).frame(0).position(Vector3(0, 0, 10)).volume(1).get(),
                             ImageBuilder().uid(5).instance(0).frame(0).position(Vector3(0, 0, 11)).volume(1).get(),
                             ImageBuilder().uid(4).instance(8).frame(0).position(Vector3(0, 0, 12)).volume(1).get(),
                             ImageBuilder().uid(10).instance(1).frame(5).position(Vector3(0, 0, -1)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(0).position(Vector3(0, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(1).position(Vector3(1, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(2).position(Vector3(1, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(3).position(Vector3(0, 0, 0)).volume(2).get(),
                             ImageBuilder().uid(10).instance(1).frame(4).position(Vector3(0, 0, 1)).volume(2).get()};
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
        QList<QList<bool>> canBeSpatiallySorted{{true, true}};

        QTest::newRow("Case 14: multi-volume, each volume with different criteria (1 distance, 2 frame number)")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // One series with 2 volumes and another with 1 volume, all multi-frame
        QList<Image*> file1{
            ImageBuilder().uid(1).instance(1).frame(0).size(10, 10).position(Vector3(10, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(1).size(10, 10).position(Vector3(10, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(2).size(10, 10).position(Vector3(10, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(3).size(10, 10).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(4).size(10, 10).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get(),
            ImageBuilder().uid(1).instance(1).frame(5).size(10, 10).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1)
                          .get()
        };
        QList<Image*> file2{
            ImageBuilder().uid(2).instance(2).frame(0).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(1).position(Vector3(-0.61628417, 0, -7.04416026))
                          .orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(2).position(Vector3(4.05579788, 0, -5.79227965))
                          .orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(3).position(Vector3(6.83012702, 0, -1.83012702))
                          .orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(4).position(Vector3(6.40856382, 0, 2.98836239))
                          .orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(5).position(Vector3(2.98836239, 0, 6.40856382))
                          .orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(6).position(Vector3(-1.83012702, 0, 6.83012702))
                          .orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(7).position(Vector3(-5.79227965, 0, 4.05579788))
                          .orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0)).volume(1).get(),
            ImageBuilder().uid(2).instance(2).frame(8).position(Vector3(-7.04416026, 0, -0.61628417))
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
        std::random_shuffle(file2.begin(), file2.end());
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

        QTest::newRow("Case 15: multi-series and multi-volume, each case with different criteria (1.1 frame number, 1.2 distance, 2.1 frame number)")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Note: created with unrealistic properties (single frame images, non-consecutive frame numbers, etc.) because it's shorter
        QList<QList<Image*>> sortedImages{{ImageBuilder().uid(2).instance(2).frame(6).acquisition(3).position(Vector3(0, 0, -5)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(64).acquisition(3).position(Vector3(0, 0, -2)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(44).acquisition(3).position(Vector3(0, 0, -1)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(4).acquisition(3).position(Vector3(0, 0, 8)).get(),
                                           ImageBuilder().uid(2).instance(2).frame(100).acquisition(3).position(Vector3(0, 0, 10)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(6).acquisition(2).position(Vector3(0, 0, -4)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(9).acquisition(2).position(Vector3(0, 0, 0)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(20).acquisition(2).position(Vector3(0, 0, 2)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(10).acquisition(2).position(Vector3(0, 0, 7)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(5).acquisition(1).position(Vector3(0, 0, 1)).get(),
                                           ImageBuilder().uid(1).instance(1).frame(0).acquisition(1).position(Vector3(0, 0, 6)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 16: different instance numbers, frame numbers and acquisitions "
                      "-> group by acquisition then order by 'distance' in each acquisition and order acquisitions by minimum 'distance'")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
        // Note that this is an unrealistic case, it can't happen at this moment because VolumeFillerStep would create separate volumes, but better to have it
        // covered just in case it becomes possible in the future
    }

    {
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().instance(1).position(Vector3(0, 12, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(2).position(Vector3(0, 11, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(3).position(Vector3(0, 10, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(4).position(Vector3(0, 2, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(5).position(Vector3(0, 1, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(6).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, 1)).get(),
            ImageBuilder().instance(7).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(8).position(Vector3(-0.61628417, 0, -7.04416026)).orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(9).position(Vector3(4.05579788, 0, -5.79227965)).orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(10).position(Vector3(6.83012702, 0, -1.83012702)).orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(11).position(Vector3(6.40856382, 0, 2.98836239)).orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(12).position(Vector3(2.98836239, 0, 6.40856382)).orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(13).position(Vector3(-1.83012702, 0, 6.83012702)).orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(14).position(Vector3(-5.79227965, 0, 4.05579788)).orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(15).position(Vector3(-7.04416026, 0, -0.61628417)).orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0))
                          .get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 17: stacks and rotational in the same series (stacks with a normal not contained in the rotational) -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().instance(1).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(2).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(3).position(Vector3(0, 0, 2)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(4).position(Vector3(0, 0, 10)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(5).position(Vector3(0, 0, 11)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(6).position(Vector3(0, 0, 12)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(7).position(Vector3(-5, 0, -5)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(8).position(Vector3(-0.61628417, 0, -7.04416026)).orientation(QVector3D(0.76604444f, 0, 0.64278761f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(9).position(Vector3(4.05579788, 0, -5.79227965)).orientation(QVector3D(0.17364818f, 0, 0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(10).position(Vector3(6.83012702, 0, -1.83012702)).orientation(QVector3D(-0.5f, 0, 0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(11).position(Vector3(6.40856382, 0, 2.98836239)).orientation(QVector3D(-0.93969262f, 0, 0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(12).position(Vector3(2.98836239, 0, 6.40856382)).orientation(QVector3D(-0.93969262f, 0, -0.34202014f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(13).position(Vector3(-1.83012702, 0, 6.83012702)).orientation(QVector3D(-0.5f, 0, -0.86602540f), QVector3D(0, 1, 0)).get(),
            ImageBuilder().instance(14).position(Vector3(-5.79227965, 0, 4.05579788)).orientation(QVector3D(0.17364818f, 0, -0.98480775f), QVector3D(0, 1, 0))
                          .get(),
            ImageBuilder().instance(15).position(Vector3(-7.04416026, 0, -0.61628417)).orientation(QVector3D(0.76604444f, 0, -0.64278761f), QVector3D(0, 1, 0))
                          .get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
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

        QTest::newRow("Case 19: 1 position, 3 phases -> order by frame number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // This is copied from a real CT localizer
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().uid(0).instance(1).acquisition(1).position(Vector3(0, -175.7, 150.01875)).orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).get(),
            ImageBuilder().uid(1).instance(1).acquisition(0).position(Vector3(-250, 74.3, 150.01875)).orientation(QVector3D(1, 0, 0), QVector3D(0, 0, -1)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 20: 2 images with same instance number and different acquisition number and orientation -> order by SOP Instance UID")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of first series of study from #351 comment 1 (DimensionIndexValues available)
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().uid(0).instance(1).frame(0).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(11, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).dimensionIndexValues({1, 1, 1}).get(),
                ImageBuilder().uid(0).instance(1).frame(1).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(0, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).dimensionIndexValues({1, 2, 2}).get(),
                ImageBuilder().uid(0).instance(1).frame(2).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-11, -150, 150))
                              .orientation(QVector3D(0, 1, 0), QVector3D(0, 0, -1)).dimensionIndexValues({1, 3, 3}).get(),
                ImageBuilder().uid(0).instance(1).frame(3).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, -16.880112528800, 145.811029553413))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).dimensionIndexValues({2, 1, 4}).get(),
                ImageBuilder().uid(0).instance(1).frame(4).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, -6.0472267866134, 147.721159458160))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).dimensionIndexValues({2, 2, 5}).get(),
                ImageBuilder().uid(0).instance(1).frame(5).size(256, 256).spacing(1.171875, 1.171875)
                              .position(Vector3(-150, 4.78565895557403, 149.631289362907))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 0.17364817857742f, -0.9848077297210f)).dimensionIndexValues({2, 3, 6}).get(),
                ImageBuilder().uid(0).instance(1).frame(6).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, -20))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).dimensionIndexValues({3, 1, 7}).get(),
                ImageBuilder().uid(0).instance(1).frame(7).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, 0))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).dimensionIndexValues({3, 2, 8}).get(),
                ImageBuilder().uid(0).instance(1).frame(8).size(256, 256).spacing(1.171875, 1.171875).position(Vector3(-150, -150, 20))
                              .orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).dimensionIndexValues({3, 3, 9}).get()
        }};
        QList<QList<Image*>> arrivingImages(sortedImages);
        std::random_shuffle(arrivingImages[0].begin(), arrivingImages[0].end());
        Series *series = new Series();

        foreach (Image *image, arrivingImages[0])
        {
            series->addImage(image);
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 21: different orientations not rotational (survey) -> order by dimension index values")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of series 501 of study from #477 (DimensionIndexValues available)
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().uid(0).instance(1).frame(0).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(18.9671724950894, -179.84025764465, -592.58819651603))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({1, 1, 1}).get(),
                ImageBuilder().uid(0).instance(1).frame(1).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(10.1672152196988, -179.84025764465, -592.56066966056))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({1, 2, 2}).get(),
                ImageBuilder().uid(0).instance(1).frame(2).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(1.36725794430822, -179.84025764465, -592.53308176994))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({1, 3, 3}).get(),
                ImageBuilder().uid(0).instance(1).frame(3).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-7.4326988542452, -179.84025764465, -592.50549387931))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({1, 4, 4}).get(),
                ImageBuilder().uid(0).instance(1).frame(4).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-16.232656606473, -179.84025764465, -592.47796702384))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({1, 5, 5}).get(),
                ImageBuilder().uid(0).instance(1).frame(5).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(19.9070910131558, -179.84025764465, -292.58969187736))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({2, 1, 6}).get(),
                ImageBuilder().uid(0).instance(1).frame(6).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(11.1071327840909, -179.84025764465, -292.56210398674))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({2, 2, 7}).get(),
                ImageBuilder().uid(0).instance(1).frame(7).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(2.30717622395604, -179.84025764465, -292.53454661369))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({2, 3, 8}).get(),
                ImageBuilder().uid(0).instance(1).frame(8).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-6.4927808130159, -179.84025764465, -292.50698924064))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({2, 4, 9}).get(),
                ImageBuilder().uid(0).instance(1).frame(9).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-15.292738088406, -179.84025764465, -292.47940135002))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({2, 5, 10}).get(),
                ImageBuilder().uid(0).instance(1).frame(10).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(20.8470076238736, -179.84025764465, 7.40884327888488))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({3, 1, 11}).get(),
                ImageBuilder().uid(0).instance(1).frame(11).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(12.0470513021573, -179.84025764465, 7.43641591072082))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({3, 2, 12}).get(),
                ImageBuilder().uid(0).instance(1).frame(12).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(3.24709450360387, -179.84025764465, 7.46398854255676))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({3, 3, 13}).get(),
                ImageBuilder().uid(0).instance(1).frame(13).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-5.5528622949495, -179.84025764465, 7.4915611743927))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({3, 4, 14}).get(),
                ImageBuilder().uid(0).instance(1).frame(14).size(512, 512).spacing(0.5859375, 0.5859375)
                              .position(Vector3(-14.352819570340, -179.84025764465, 7.51913380622863))
                              .orientation(QVector3D(0, 1, 0), QVector3D(-0.0031330608762f, 0, -0.9999951124191f)).dimensionIndexValues({3, 5, 15}).get()
        }};
        QList<QList<Image*>> arrivingImages(sortedImages);
        std::random_shuffle(arrivingImages[0].begin(), arrivingImages[0].end());
        Series *series = new Series();

        foreach (Image *image, arrivingImages[0])
        {
            series->addImage(image);
        }

        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 22: 3 parallel stacks -> order by dimension index values")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(3).position(Vector3(0, 0, 1)).orientation(QVector3D(0, 1, 0), QVector3D(-1, 0, 0)).get()}};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 23: 1 position with 2 phases, another position with 2 orientations but same normal "
                      "-> irregular phases, order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{ImageBuilder().instance(0).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(1).position(Vector3(0, 0, 1)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
                                           ImageBuilder().instance(2).position(Vector3(0, 0, 0)).orientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)).get(),
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
                      "-> irregular phases, order by instance number")
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

    {
        // Recreation of series 80476 of study 1.2.840.113704.1.111.7168.1154688850.1 from qms://doc/89717081
        QList<QList<Image*>> sortedImages{{
            ImageBuilder().instance(1).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.988452012f, 0.142040927f, -0.0527919962f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(2).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.784258208f, 0.505957755f, -0.359090258f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(3).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.36992305f, 0.734303611f, -0.569170575f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(4).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.14353269f, 0.765893408f, -0.626742096f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(5).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.618528962f, 0.592262684f, -0.516378578f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(6).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.927790898f, 0.259935653f, -0.267651838f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(7).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.988452012f, -0.142040927f, 0.0527919962f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(8).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.784258208f, -0.505957755f, 0.359090258f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(9).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(-0.36992305f, -0.734303611f, 0.569170575f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(10).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.14353269f, -0.765893408f, 0.626742096f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(11).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.618528962f, -0.592262684f, 0.516378578f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get(),
            ImageBuilder().instance(12).size(512, 512).spacing(0, 0).position(Vector3(-114.733201, 45.9815706, 216.146124))
                          .orientation(QVector3D(0.927790898f, -0.259935653f, 0.267651838f), QVector3D(0.0485899864f, -0.627081863f, -0.777436396f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 26: rotational with all images in the same position and null pixel spacing -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of series 1305 of study 3050008668313000 from qms://doc/89717081
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(1).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-82.120732324382, 90.7569741284885, 4.56965896266790))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.00357209711373f, -0.9911348391031f, 0.13281178727155f)).get(),
                ImageBuilder().instance(2).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-83.100955511689, 91.2259629691298, 21.2588747194885))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.01582488695506f, -0.9969971996111f, -0.0758034096887f)).get(),
                ImageBuilder().instance(3).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-84.025848819301, 88.2090668700623, 37.6830526564280))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.02738605330022f, -0.9592859983727f, -0.2811056339004f)).get(),
                ImageBuilder().instance(4).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-84.854989971548, 81.8381386674889, 53.1243773856925))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.03775031770330f, -0.8796493958406f, -0.4741221930162f)).get(),
                ImageBuilder().instance(5).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-85.552141521413, 72.3916184949674, 66.9079889209162))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.04646471207662f, -0.7615678936840f, -0.6464173372065f)).get(),
                ImageBuilder().instance(6).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-86.086834600865, 60.2823646134700, 78.4314772969035))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.05314837556977f, -0.6102022201653f, -0.7904609419063f)).get(),
                ImageBuilder().instance(7).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-86.435700556588, 46.0396095242650, 87.1912107757914))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.05750920001631f, -0.4321677815503f, -0.8999576103924f)).get(),
                ImageBuilder().instance(8).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-86.583492272274, 30.2858299690724, 92.8043469706879))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.05935659646238f, -0.2352455371104f, -0.9701218128286f)).get(),
                ImageBuilder().instance(9).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-86.523750540764, 13.7095417094754, 95.0255648950601))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.05860982481850f, -0.0280419338654f, -0.9978870368833f)).get(),
                ImageBuilder().instance(10).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-86.259086362396, -2.9647919157253, 93.7577866670267))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.05530152258890f, 0.18038723644956f, -0.9820398090329f)).get(),
                ImageBuilder().instance(11).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-85.801066831766, -19.008422514778, 89.0564202786278))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.04957627845603f, 0.38093261893773f, -0.9232727291779f)).get(),
                ImageBuilder().instance(12).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-85.169709600183, -33.720166444624, 81.1269380006952))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.04168431306124f, 0.56482941806081f, -0.8241542007037f)).get(),
                ImageBuilder().instance(13).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-84.392608008256, -46.457049900750, 70.3158962586521))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.03197054316216f, 0.72404046126237f, -0.6890161789282f)).get(),
                ImageBuilder().instance(14).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-83.503725124499, -56.662409957773, 57.0957894537598))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.02085950711519f, 0.85160746197516f, -0.5237648438670f)).get(),
                ImageBuilder().instance(15).size(512, 512).spacing(0.3125, 0.3125).position(Vector3(-82.541909396266, -63.890223410452, 42.0443996905027))
                              .orientation(QVector3D(0.99823333374883f, 0.01141113638921f, 0.05830949630209f),
                                           QVector3D(0.00883681051228f, 0.94195513013365f, -0.3356224718263f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};
        QList<QList<bool>> canBeSpatiallySorted{{false}};

        QTest::newRow("Case 27: rotational with all images with the same center -> order by instance number")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().instance(1).acquisition(2).stackId("asdf").position(Vector3(0, 0, 0)).get(),
                ImageBuilder().instance(5).acquisition(1).stackId("foo").position(Vector3(0, 0, 0)).get(),
                ImageBuilder().instance(2).acquisition(1).stackId("foo").position(Vector3(0, 0, 6)).get(),
                ImageBuilder().instance(4).acquisition(1).stackId("1").position(Vector3(0, 0, 1)).get(),
                ImageBuilder().instance(8).acquisition(1).stackId("_").position(Vector3(0, 0, 3)).get(),
                ImageBuilder().instance(9).acquisition(1).stackId("<( -'.'- )>").position(Vector3(0, 0, 3)).get(),
                ImageBuilder().instance(0).acquisition(1).stackId("<( -'.'- )>").position(Vector3(0, 0, 5)).get(),
                ImageBuilder().instance(3).acquisition(1).stackId("<( -'.'- )>").position(Vector3(0, 0, 7)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 28: multiple acquisitions and stacks with intersection "
                      "-> order images spatially in each stack, then stacks spatially in each acquistion, then acquisitions spatially")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }

    {
        // Recreation of series 801 of first study from #2968
        QList<QList<Image*>> sortedImages{{
                ImageBuilder().uid(55).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.557516426157, -35.189993438954, -458.14321481946))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(34).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.548456043314, -37.208792267079, -454.2336994386))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(37).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.539395660471, -39.227591095204, -450.32415354016))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(52).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.530335277628, -41.246389923329, -446.4146381593))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(36).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.521274894785, -43.265188751454, -442.50512277844))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(69).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.512214511942, -45.283987579579, -438.59557688001))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),
                ImageBuilder().uid(21).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-96.503154129099, -47.302786407704, -434.68606149915))
                              .orientation(QVector3D(0.99999767541885f, 0.00150616886094f, -0.0015397614333f),
                                           QVector3D(-0.0006318031810f, 0.88852906227111f, 0.45881986618041f)).get(),

                ImageBuilder().uid(90).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.358643890754, -58.852513345146, -394.9181777122))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(84).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.424341560737, -59.261223824882, -390.53768454814))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(46).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.490039707557, -59.669934304618, -386.15722190166))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(64).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.555737854377, -60.078644784355, -381.77672873759))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(82).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.621436001197, -60.487355264091, -377.39623557353))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(45).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.687134148017, -60.896065743827, -373.01577292705))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),
                ImageBuilder().uid(62).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.752831818, -61.304776223564, -368.63527976299))
                              .orientation(QVector3D(0.99953323602676f, 0.02515283972024f, 0.01733772829174f),
                                           QVector3D(-0.0266517568379f, 0.99535870552062f, 0.09246993064880f)).get(),

                ImageBuilder().uid(53).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-91.850712101613, -61.030395164618, -338.71371172648))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(31).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-91.919732372915, -60.484050407538, -334.34829424601))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(7).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-91.988753121053, -59.93770183576, -329.98290728312))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(13).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.057773869191, -59.39135707868, -325.61748980265))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(44).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.12679461733, -58.8450123216, -321.25207232218))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(48).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.195815365468, -58.298663749823, -316.88668535929))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),
                ImageBuilder().uid(1).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.26483563677, -57.752318992743, -312.52126787882))
                              .orientation(QVector3D(0.99935358762741f, 0.03404841944575f, 0.01153932325541f),
                                           QVector3D(-0.0323478654026f, 0.99167668819427f, -0.1246234104037f)).get(),

                ImageBuilder().uid(0).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.112488214085, -37.410457172356, -255.45636625813))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(59).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.18384689481, -36.2248492622, -251.21970243977))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(72).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.255205098698, -35.039237537347, -246.98306913899))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(3).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.326563779423, -33.85362962719, -242.74640532063))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(94).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.397922460148, -32.668021717034, -238.50974150227))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(79).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.469280664036, -31.482413806878, -234.27310820149))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get(),
                ImageBuilder().uid(66).size(336, 336).spacing(0.59523808956146, 0.59523808956146)
                              .position(Vector3(-92.540639344761, -30.296798267327, -230.03644438313))
                              .orientation(QVector3D(0.99936103820800f, 0.03504495322704f, 0.00702517898753f),
                                           QVector3D(-0.0318509601056f, 0.96237462759017f, -0.2698529362678f)).get()
        }};
        QList<QList<Image*>> arrivingImages = imagesOneByOne(sortedImages);
        Series *series = shuffleAndCreateSeries(arrivingImages);
        QList<Series*> seriesList{series};
        QList<QList<int>> orderNumbers{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}};
        QList<QList<bool>> canBeSpatiallySorted{{true}};

        QTest::newRow("Case 29: 4 stacks of 7 slices each without Stack ID -> autodetect stacks, order images spatially in each stack, then stacks spatially")
                << arrivingImages << seriesList << sortedImages << orderNumbers << canBeSpatiallySorted;
    }
}

DECLARE_TEST(test_OrderImagesFillerStep)

#include "test_orderimagesfillerstep.moc"
