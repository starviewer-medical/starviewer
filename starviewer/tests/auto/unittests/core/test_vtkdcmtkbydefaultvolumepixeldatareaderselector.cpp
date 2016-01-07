#include "autotest.h"
#include "vtkdcmtkbydefaultvolumepixeldatareaderselector.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_VtkDcmtkByDefaultVolumePixelDataReaderSelector : public QObject {

    Q_OBJECT

private slots:

    void selectVolumePixelDataReader_ShouldReturnExpectedReaderType_data();
    void selectVolumePixelDataReader_ShouldReturnExpectedReaderType();

};

Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(VolumePixelDataReaderFactory::PixelDataReaderType)

void test_VtkDcmtkByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader_ShouldReturnExpectedReaderType_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<VolumePixelDataReaderFactory::PixelDataReaderType>("expectedReaderType");

    // Default image:
    // - SOP instance UID: ""

    // Conditions:
    // - SOP instance UID = MHDImage

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        QTest::newRow("volume with 2 default images | !mhd -> VTK-DCMTK") << volume << VolumePixelDataReaderFactory::VTKDCMTKPixelDataReader;
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->setSOPInstanceUID("MHDImage");
        volume->getImage(1)->setSOPInstanceUID("MHDImage");
        QTest::newRow("MHDImage | mhd -> ITK-GDCM") << volume << VolumePixelDataReaderFactory::ITKGDCMPixelDataReader;
    }
}

void test_VtkDcmtkByDefaultVolumePixelDataReaderSelector::selectVolumePixelDataReader_ShouldReturnExpectedReaderType()
{
    QFETCH(Volume*, volume);
    QFETCH(VolumePixelDataReaderFactory::PixelDataReaderType, expectedReaderType);

    VtkDcmtkByDefaultVolumePixelDataReaderSelector selector;

    QCOMPARE(selector.selectVolumePixelDataReader(volume), expectedReaderType);

    VolumeTestHelper::cleanUp(volume);
}

DECLARE_TEST(test_VtkDcmtkByDefaultVolumePixelDataReaderSelector)

#include "test_vtkdcmtkbydefaultvolumepixeldatareaderselector.moc"
