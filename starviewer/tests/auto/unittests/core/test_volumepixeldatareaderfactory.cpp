/*@
    "name": "test_VolumePixelDataReaderFactory",
    "requirements": ["archive.dicom_format", "archive.other_formats"]
 */

#include "autotest.h"
#include "volumepixeldatareaderfactory.h"

#include "coresettings.h"
#include "image.h"
#include "series.h"
#include "testingsettings.h"
#include "volume.h"
#include "volumepixeldatareader.h"
#include "volumepixeldatareaderitkdcmtk.h"
#include "volumepixeldatareaderitkgdcm.h"
#include "volumepixeldatareadervtkdcmtk.h"
#include "volumepixeldatareadervtkgdcm.h"
#include "volumetesthelper.h"

using namespace testing;
using namespace udg;

class test_VolumePixelDataReaderFactory : public QObject {

    Q_OBJECT

private slots:

    void getReader_ShouldReturnExpectedReaderType_data();
    void getReader_ShouldReturnExpectedReaderType();

    void cleanupTestCase();
};

Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(TestingSettings)

void test_VolumePixelDataReaderFactory::getReader_ShouldReturnExpectedReaderType_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<QString>("expectedReaderType");

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        QTest::newRow("default -> VTK-DCMTK") << volume << TestingSettings() << QString(typeid(VolumePixelDataReaderVTKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForcedImageReaderLibrary, "itk");
        QTest::newRow("force itk -> ITK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForcedImageReaderLibrary, "vtk");
        QTest::newRow("force vtk -> VTK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForcedImageReaderLibrary, "itkdcmtk");
        QTest::newRow("force itkdcmtk -> ITK-DCMTK") << volume << testingSettings << QString(typeid(VolumePixelDataReaderITKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForcedImageReaderLibrary, "vtkdcmtk");
        QTest::newRow("force vtkdcmtk -> VTK-DCMTK") << volume << testingSettings << QString(typeid(VolumePixelDataReaderVTKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForcedImageReaderLibrary, "foo");
        QTest::newRow("force invalid -> VTK-DCMTK") << volume << testingSettings << QString(typeid(VolumePixelDataReaderVTKDCMTK).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CT");
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities, "CT");
        QTest::newRow("force itk for one modality -> ITK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CT");
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities, "CT\\MR");
        QTest::newRow("force itk for n modalities -> ITK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CT");
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities, "CT");
        QTest::newRow("force vtk for one modality -> VTK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

        {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("MR");
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities, "CT\\MR");
        QTest::newRow("force vtk for n modalities -> VTK-GDCM") << volume << testingSettings << QString(typeid(VolumePixelDataReaderVTKGDCM).name());
    }

    {
        Volume *volume = VolumeTestHelper::createVolume(2);
        volume->getImage(0)->getParentSeries()->setModality("CT");
        TestingSettings testingSettings;
        testingSettings.setValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities, "CT");
        testingSettings.setValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities, "CT");
        QTest::newRow("modality-force itk has precedence over modality-force vtk -> ITK-GDCM")
            << volume << testingSettings << QString(typeid(VolumePixelDataReaderITKGDCM).name());
    }
}

void test_VolumePixelDataReaderFactory::getReader_ShouldReturnExpectedReaderType()
{
    QFETCH(Volume*, volume);
    QFETCH(TestingSettings, testingSettings);
    QFETCH(QString, expectedReaderType);

    Settings::setStaticTestingSettings(&testingSettings);

    VolumePixelDataReaderFactory factory;
    factory.setVolume(volume);
    VolumePixelDataReader *reader = factory.getReader();

    QCOMPARE(QString(typeid(*reader).name()), expectedReaderType);

    delete reader;
    VolumeTestHelper::cleanUp(volume);
}

void test_VolumePixelDataReaderFactory::cleanupTestCase()
{
    Settings::setStaticTestingSettings(nullptr);
}

DECLARE_TEST(test_VolumePixelDataReaderFactory)

#include "test_volumepixeldatareaderfactory.moc"
