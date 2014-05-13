#include "autotest.h"
#include "itkgdcmbydefaultvolumepixeldatareaderselector.h"

#include "image.h"
#include "series.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_ItkGdcmByDefaultVolumePixelDataReaderSelector : public QObject {

    Q_OBJECT

private slots:

    void selectVolumePixelDataReader_ShouldReturnExpectedReaderType_data();
    void selectVolumePixelDataReader_ShouldReturnExpectedReaderType();

};

Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(VolumePixelDataReaderFactory::PixelDataReaderType)

void test_ItkGdcmByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader_ShouldReturnExpectedReaderType_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<VolumePixelDataReaderFactory::PixelDataReaderType>("expectedReaderType");

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

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        QTest::newRow("volume with 2 default images | !diff, !color, !avoid -> ITK-GDCM") << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        QTest::newRow("different rows | diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setColumns(1);
        volume->getImage(1)->setColumns(2);
        QTest::newRow("different columns | diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(0)->setColumns(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(1)->setColumns(2);
        QTest::newRow("different rows & different columns | diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("color | !diff, color, !avoid -> VTK-GDCM")
            << volume << VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        QTest::newRow("different size & color | diff, color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("non-volumetric modality | !diff, !color, avoid -> VTK-GDCM")
            << volume << VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsStored(12);
        volume->getImage(1)->setBitsStored(12);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits stored != 16 & non-volumetric modality | !diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsAllocated(32);
        volume->getImage(1)->setBitsAllocated(32);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits allocated != 16 & non-volumetric modality | !diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setBitsAllocated(8);
        volume->getImage(0)->setBitsStored(8);
        volume->getImage(1)->setBitsAllocated(8);
        volume->getImage(1)->setBitsStored(8);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("bits allocated != 16 & bits stored != 16 & non-volumetric modality | !diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("MHDImage & non-volumetric modality | !diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        QTest::newRow("MHDImage | !diff, !color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("different size & non-volumetric modality | diff, !color, avoid -> VTK-GDCM")
            << volume << VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("color & non-volumetric modality | !diff, color, !avoid -> VTK-GDCM")
            << volume << VolumePixelDataReaderFactory::VTKGDCMPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setRows(1);
        volume->getImage(1)->setRows(2);
        volume->getImage(0)->setPhotometricInterpretation("RGB");
        volume->getImage(1)->setPhotometricInterpretation("RGB");
        volume->getImage(0)->getParentSeries()->setModality("CR");
        QTest::newRow("different size & color & non-volumetric modality | diff, color, !avoid -> ITK-GDCM")
            << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
}

void test_ItkGdcmByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader_ShouldReturnExpectedReaderType()
{
    QFETCH(Volume*, volume);
    QFETCH(VolumePixelDataReaderFactory::PixelDataReaderType, expectedReaderType);

    ItkGdcmByDefaultVolumePixelDataReaderSelector selector;

    QCOMPARE(selector.selectVolumePixelDataReader(volume), expectedReaderType);

    VolumeTestHelper::cleanUp(volume);
}

DECLARE_TEST(test_ItkGdcmByDefaultVolumePixelDataReaderSelector)

#include "test_itkgdcmbydefaultvolumepixeldatareaderselector.moc"
