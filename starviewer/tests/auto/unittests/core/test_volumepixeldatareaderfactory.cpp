#include "autotest.h"
#include "volumepixeldatareaderfactory.h"

#include "image.h"
#include "series.h"
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
    // - rows: 0
    // - columns: 0
    // - photometric interpretation: "MONOCHROME2"
    // - bits allocated: 16
    // - bits stored: 16
    // - SOP instance UID: ""

    // Default series:
    // - modality: "OT"

    // Conditions:
    // - different size images (different rows or different columns or both)
    // - color images (photometric interpretation != MONOCHROME)
    // - bits allocated = 16
    // - bits stored = 16
    // - SOP instance UID = MHDImage
    // - modality = "CR" or "RF" or "DX" or "MG" or "OP" or "US" or "ES" or "NM" or "DT" or "PT" or "XA" or "XC"
    // - multiframe

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        QTest::newRow("volume with 2 default images | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        QTest::newRow("different rows | diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setColumns(1);
        volume->getImage(1)->setColumns(2);
        QTest::newRow("different columns | diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(0)->setColumns(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(1)->setColumns(2);
        QTest::newRow("different rows & different columns | diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("color | !diff, color, !avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("different size & color | diff, color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("non-volumetric modality | !diff, !color, avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(12);
        volume->getImage(1)->setBitsStored(12);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits stored != 16 & non-volumetric modality | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsAllocated(32);
        volume->getImage(1)->setBitsAllocated(32);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits allocated != 16 & non-volumetric modality | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsAllocated(8);
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setBitsAllocated(8);
        volume->getImage(1)->setBitsStored(8);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits allocated != 16 & bits stored != 16 & non-volumetric modality | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("MHDImage & non-volumetric modality | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        QTest::newRow("MHDImage | !diff, !color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("different size & non-volumetric modality | diff, !color, avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("color & non-volumetric modality | !diff, color, !avoid, !multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("different size & color & non-volumetric modality | diff, color, !avoid, !multiframe -> ITK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        QTest::newRow("multiframe | !diff, !color, !avoid, multiframe -> ITK-DCMTK")
            << volume << QString(typeid(VolumePixelDataReaderITKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("color & multiframe | !diff, color, !avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("non-volumetric modality & multiframe | !diff, !color, avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createMultiframeVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("color & non-volumetric modality & multiframe | !diff, color, !avoid, multiframe -> VTK-GDCM")
            << volume << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
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
