#include "autotest.h"
#include "volumepixeldatareaderfactory.h"

#include "image.h"
#include "volume.h"
#include "volumepixeldatareader.h"
#include "volumepixeldatareaderitkdcmtk.h"
#include "volumepixeldatareaderitkgdcm.h"
#include "volumepixeldatareadervtkgdcm.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_VolumePixelDataReaderFactory : public QObject {

    Q_OBJECT

private slots:

    void getReader_ShouldReturnExpectedReaderType_data();
    void getReader_ShouldReturnExpectedReaderType();

};

Q_DECLARE_METATYPE(Volume*)

void test_VolumePixelDataReaderFactory::getReader_ShouldReturnExpectedReaderType_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<QString>("expectedReaderType");

    // Default image:
    // - SOP instance UID: ""
    // - photometric interpretation: "MONOCHROME2"
    // - bits stored: 16
    // - pixel representation: 0
    // - rescale slope: 1.0
    // - rescale intercept: 0.0

    // Conditions:
    // - SOP instance UID = MHDImage
    // - color images (photometric interpretation != MONOCHROME)
    // - rescale slope * bits (consider pixel representation) + rescale intercept > 32767.0
    // - multiframe

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        QTest::newRow("volume with 2 default images | !mhd, !color, avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("default reader | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("MHDImage (& !avoid) | mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("color (& !avoid) | !mhd, color, !avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("MHDImage & color (& !avoid) | mhd, color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(15);
        volume->getImage(1)->setBitsStored(15);
        QTest::newRow("bits stored = 15 | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPixelRepresentation(1);
        volume->getImage(1)->setPixelRepresentation(1);
        QTest::newRow("pixel representation = 1 | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRescaleSlope(0.4);
        volume->getImage(1)->setRescaleSlope(0.4);
        QTest::newRow("rescale slope = 0.4 | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRescaleIntercept(-32768.0);
        volume->getImage(1)->setRescaleIntercept(-32768.0);
        QTest::newRow("rescale intercept = -32768 | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(0)->setRescaleSlope(10.0);
        volume->getImage(0)->setRescaleIntercept(-1000.0);
        volume->getImage(1)->setBitsStored(8);
        volume->getImage(1)->setRescaleSlope(10.0);
        volume->getImage(1)->setRescaleIntercept(-1000.0);
        QTest::newRow("small max value | !mhd, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(12);
        volume->getImage(0)->setRescaleSlope(100.0);
        volume->getImage(1)->setBitsStored(12);
        volume->getImage(1)->setRescaleSlope(100.0);
        QTest::newRow("big max value | !mhd, !color, avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        QTest::newRow("MHDImage (& avoid) | mhd, !color, avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("color (& avoid) | !mhd, color, avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("MHDImage & color (& avoid) | mhd, color, avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("multiframe (& !avoid)| !mhd, !color, !avoid, multiframe -> ITK-DCMTK")
            << volume << QString(typeid(VolumePixelDataReaderITKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("MHDImage & multiframe (& !avoid) | mhd, !color, !avoid, multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("color & multiframe (& !avoid) | !mhd, color, !avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setBitsStored(8);
        QTest::newRow("MHDImage & color & multiframe (&!avoid) | mhd, color, !avoid, multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        QTest::newRow("multiframe (& avoid) | !mhd, !color, avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        QTest::newRow("MHDImage & multiframe (& avoid) | mhd, !color, avoid, multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("color & multiframe (& avoid) | !mhd, color, avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("MHDImage & color & multiframe (& avoid) | mhd, color, avoid, multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }
}

void test_VolumePixelDataReaderFactory::getReader_ShouldReturnExpectedReaderType()
{
    QFETCH(Volume*, volume);
    QFETCH(QString, expectedReaderType);

    VolumePixelDataReaderFactory factory(volume);
    VolumePixelDataReader *reader = factory.getReader();

    QCOMPARE(QString(typeid(*reader).name()), expectedReaderType);

    delete reader;
    VolumeTestHelper::cleanUp(volume);
}

DECLARE_TEST(test_VolumePixelDataReaderFactory)

#include "test_volumepixeldatareaderfactory.moc"
