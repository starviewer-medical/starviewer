#include "autotest.h"
#include "pixelspacingamenderpostprocessor.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_PixelSpacingAmenderPostProcessor : public QObject {
Q_OBJECT

private slots:
    void postprocess_BehavesAsExpected_data();
    void postprocess_BehavesAsExpected();
};

Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(double*)

void test_PixelSpacingAmenderPostProcessor::postprocess_BehavesAsExpected_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double*>("expectedPixelSpacing");
    
    {
        Volume *volume = VolumeTestHelper::createVolume();
        volume->setParent(this);
        volume->getPixelData()->setSpacing(0.5, 0.5, 2.0);
        double *expectedPixelSpacing = new double[2];
        expectedPixelSpacing[0] = 0.5;
        expectedPixelSpacing[1] = 0.5;
        
        QTest::newRow("Volume with no images") << volume << expectedPixelSpacing;
    }
    
    {
        Volume *volume = VolumeTestHelper::createVolume(1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(.5, .5, 2.0);
        
        double *expectedPixelSpacing = new double[2];
        expectedPixelSpacing[0] = .5;
        expectedPixelSpacing[1] = .5;
        
        QTest::newRow("Pixel Spacing is not present in image") << volume << expectedPixelSpacing;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        volume->getImage(0)->setPixelSpacing(2.5, 1.0);
        double *expectedPixelSpacing = new double[2];
        expectedPixelSpacing[0] = 2.5;
        expectedPixelSpacing[1] = 1.0;
        
        QTest::newRow("Volume spacing (X) differs from image") << volume << expectedPixelSpacing;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        volume->getImage(0)->setPixelSpacing(1.0, 2.5);
        double *expectedPixelSpacing = new double[2];
        expectedPixelSpacing[0] = 1.0;
        expectedPixelSpacing[1] = 2.5;
        
        QTest::newRow("Volume spacing (Y) differs from image") << volume << expectedPixelSpacing;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(1);
        volume->setParent(this);
        volume->getPixelData()->setSpacing(1.0, 1.0, 3.0);
        volume->getImage(0)->setPixelSpacing(2.5, 2.5);
        double *expectedPixelSpacing = new double[2];
        expectedPixelSpacing[0] = 2.5;
        expectedPixelSpacing[1] = 2.5;
        
        QTest::newRow("Volume spacing (X & Y) differs from image") << volume << expectedPixelSpacing;
    }
}

void test_PixelSpacingAmenderPostProcessor::postprocess_BehavesAsExpected()
{
    QFETCH(Volume*, volume);
    QFETCH(double*, expectedPixelSpacing);
    
    PixelSpacingAmenderPostProcessor postprocessor;
    postprocessor.postprocess(volume);

    QCOMPARE(volume->getSpacing()[0], expectedPixelSpacing[0]);
    QCOMPARE(volume->getSpacing()[1], expectedPixelSpacing[1]);
}

DECLARE_TEST(test_PixelSpacingAmenderPostProcessor)

#include "test_pixelspacingamenderpostprocessor.moc"
