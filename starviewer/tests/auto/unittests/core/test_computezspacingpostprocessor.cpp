#include "autotest.h"
#include "computezspacingpostprocessor.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_ComputeZSpacingPostprocessor : public QObject {

    Q_OBJECT

private slots:

    void postprocess_ShouldAssignCorrectZSpacing_data();
    void postprocess_ShouldAssignCorrectZSpacing();

    void postprocess_ShouldNotChangeZSpacing_data();
    void postprocess_ShouldNotChangeZSpacing();

};

Q_DECLARE_METATYPE(Volume*)

void test_ComputeZSpacingPostprocessor::postprocess_ShouldAssignCorrectZSpacing_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("expectedZSpacing");

    {
        Volume *volume = VolumeTestHelper::createVolume(2, 1, 2);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        double position[3] = { 0.0, 0.0, 0.0 };
        volume->getImage(0)->setImagePositionPatient(position);
        position[2] = 1.0;
        volume->getImage(1)->setImagePositionPatient(position);
        ImageOrientation orientation;
        orientation.setRowAndColumnVectors(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
        volume->getImage(0)->setImageOrientationPatient(orientation);
        volume->getImage(1)->setImageOrientationPatient(orientation);
        QTest::newRow("n slices, 1 phase") << volume << 1.0;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(4, 2, 2);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        double position[3] = { 0.0, 0.0, 0.0 };
        volume->getImage(0)->setImagePositionPatient(position);
        position[2] = 1.0;
        volume->getImage(1)->setImagePositionPatient(position);
        ImageOrientation orientation;
        orientation.setRowAndColumnVectors(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
        volume->getImage(0)->setImageOrientationPatient(orientation);
        volume->getImage(1)->setImageOrientationPatient(orientation);
        QTest::newRow("n slices, m phases") << volume << 1.0;
    }
}

void test_ComputeZSpacingPostprocessor::postprocess_ShouldAssignCorrectZSpacing()
{
    QFETCH(Volume*, volume);
    QFETCH(double, expectedZSpacing);

    ComputeZSpacingPostprocessor postprocessor;
    postprocessor.postprocess(volume);

    QCOMPARE(volume->getSpacing()[2], expectedZSpacing);
}

void test_ComputeZSpacingPostprocessor::postprocess_ShouldNotChangeZSpacing_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("expectedZSpacing");

    {
        Volume *volume = VolumeTestHelper::createVolume(1, 1, 1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        QTest::newRow("1 slice") << volume << 3.0;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2, 2, 1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        QTest::newRow("1 slice per phase") << volume << 3.0;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2, 1, 2);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        double position[3] = { 0.0, 0.0, 0.0 };
        volume->getImage(0)->setImagePositionPatient(position);
        volume->getImage(1)->setImagePositionPatient(position);
        ImageOrientation orientation;
        orientation.setRowAndColumnVectors(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
        volume->getImage(0)->setImageOrientationPatient(orientation);
        volume->getImage(1)->setImageOrientationPatient(orientation);
        QTest::newRow("slices at same position") << volume << 3.0;
    }
}

void test_ComputeZSpacingPostprocessor::postprocess_ShouldNotChangeZSpacing()
{
    QFETCH(Volume*, volume);
    QFETCH(double, expectedZSpacing);

    ComputeZSpacingPostprocessor postprocessor;
    postprocessor.postprocess(volume);

    QCOMPARE(volume->getSpacing()[2], expectedZSpacing);
}

DECLARE_TEST(test_ComputeZSpacingPostprocessor)

#include "test_computezspacingpostprocessor.moc"
