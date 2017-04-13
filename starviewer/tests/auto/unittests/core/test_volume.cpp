#include "autotest.h"

#include "volume.h"
#include "image.h"
#include "series.h"
#include "volumetesthelper.h"
#include "seriestesthelper.h"
#include "volumepixeldatatesthelper.h"
#include "imagetesthelper.h"
#include "itkandvtkimagetesthelper.h"
#include "fuzzycomparetesthelper.h"
#include "testingvolume.h"
#include "testingvolumereader.h"

#include <QVector3D>
#include <QSharedPointer>

#include <itkImageRegionConstIterator.h>

using namespace udg;
using namespace testing;

using Corners = decltype(Volume().getCorners());

class test_Volume : public QObject {
Q_OBJECT

private slots:
    void constructor_ShouldCreateMinimalVolume();

    void setData_itk_ShouldBehaveAsExpected_data();
    void setData_itk_ShouldBehaveAsExpected();

    void setData_vtk_ShouldBehaveAsExpected_data();
    void setData_vtk_ShouldBehaveAsExpected();

    void setPixelData_ShouldBehaveAsExpected_data();
    void setPixelData_ShouldBehaveAsExpected();

    void getPixelData_ShouldReturnCurrentPixelData_data();
    void getPixelData_ShouldReturnCurrentPixelData();

    void getPixelData_ShouldRead();

    void getAcquisitionPlane_ShouldReturnNotAvailable_data();
    void getAcquisitionPlane_ShouldReturnNotAvailable();

    void getAcquisitionPlane_ShouldReturnExpectedPlane_data();
    void getAcquisitionPlane_ShouldReturnExpectedPlane();

    void getCorrespondingOrthogonalPlane_ShouldReturnExpectedValues_data();
    void getCorrespondingOrthogonalPlane_ShouldReturnExpectedValues();
    
    void getStackDirection_ShouldNotModifyDirection_data();
    void getStackDirection_ShouldNotModifyDirection();

    void getStackDirection_ShouldReturnNormalVector_data();
    void getStackDirection_ShouldReturnNormalVector();

    void getStackDirection_ShouldReturnExpectedDirection_data();
    void getStackDirection_ShouldReturnExpectedDirection();

    void getPhaseImages_ShouldReturnExpectedPhaseImages_data();
    void getPhaseImages_ShouldReturnExpectedPhaseImages();

    void getOrigin_ShouldReturnExpectedOrigin_data();
    void getOrigin_ShouldReturnExpectedOrigin();

    void getDimensions_ShouldReturnExpectedDimensions_data();
    void getDimensions_ShouldReturnExpectedDimensions();

    void getCorners_ShouldReturnExpectedValues_data();
    void getCorners_ShouldReturnExpectedValues();

    void getSpacing_ShouldReturnExpectedSpacing_data();
    void getSpacing_ShouldReturnExpectedSpacing();

    void getExtent_ShouldReturnExpectedExtent_data();
    void getExtent_ShouldReturnExpectedExtent();

    void setPhases_ShouldSetValidPhasesValue_data();
    void setPhases_ShouldSetValidPhasesValue();

    void addImage_ShouldAddValidImages_data();
    void addImage_ShouldAddValidImages();

    void convertToNeutralVolume_ShouldBehaveAsExpected_data();
    void convertToNeutralVolume_ShouldBehaveAsExpected();
    
    void setImages_ShouldAddImageList_data();
    void setImages_ShouldAddImageList();

    void getImage_ShouldReturnExpectedImage_data();
    void getImage_ShouldReturnExpectedImage();

    void getPixelUnits_ShouldReturnExpectedValue_data();
    void getPixelUnits_ShouldReturnExpectedValue();

    void getImageIndex_ShouldReturnExpectedImageIndex_data();
    void getImageIndex_ShouldReturnExpectedImageIndex();

    void toString_ShouldReturnExpectedString_data();
    void toString_ShouldReturnExpectedString();

    void getScalarPointer_ShouldReturnCorrectScalarPointer_data();
    void getScalarPointer_ShouldReturnCorrectScalarPointer();

    void isMultiframe_ShouldReturnTrueForMultiframeVolumes_data();
    void isMultiframe_ShouldReturnTrueForMultiframeVolumes();
};

Q_DECLARE_METATYPE(AnatomicalPlane)
Q_DECLARE_METATYPE(QList<Image*>)
Q_DECLARE_METATYPE(QSharedPointer<Volume>)
Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(ImageOrientation)
Q_DECLARE_METATYPE(Volume::ItkImageTypePointer)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkImageData>)
Q_DECLARE_METATYPE(VolumePixelData*)
Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(OrthogonalPlane)
Q_DECLARE_METATYPE(Corners)
Q_DECLARE_METATYPE(Vector3)

void test_Volume::constructor_ShouldCreateMinimalVolume()
{
    Volume volume;

    QCOMPARE(volume.getNumberOfPhases(), 1);
    QCOMPARE(volume.getNumberOfSlicesPerPhase(), 1);
    QCOMPARE(volume.isPixelDataLoaded(), false);
    QVERIFY(volume.getPixelData() != 0);
}

void test_Volume::setData_itk_ShouldBehaveAsExpected_data()
{
    QTest::addColumn<Volume::ItkImageTypePointer>("itkData");

    {
        Volume::ItkImageTypePointer itkData = Volume::ItkImageType::New();
        QTest::newRow("default") << itkData;
    }

    {
        int dimensions[3] = { 200, 200, 200 };
        int startIndex[3] = { 0, 0, 0 };
        double spacing[3] = { 0.33, 0.33, 1.20 };
        double origin[3] = { 0.0, 0.0, 0.0 };
        QTest::newRow("random #1") << ItkAndVtkImageTestHelper::createItkImage(dimensions, startIndex, spacing, origin);
    }

    {
        int dimensions[3] = { 33, 124, 6 };
        int startIndex[3] = { 200, 169, 156 };
        double spacing[3] = { 2.2, 0.74, 1.44 };
        double origin[3] = { 48.0, 41.0, -68.0 };
        QTest::newRow("random #2") << ItkAndVtkImageTestHelper::createItkImage(dimensions, startIndex, spacing, origin);
    }
}

void test_Volume::setData_itk_ShouldBehaveAsExpected()
{
    QFETCH(Volume::ItkImageTypePointer, itkData);

    Volume volume;
    volume.setData(itkData);

    QCOMPARE(volume.isPixelDataLoaded(), true);

    Volume::ItkImageTypePointer outputItkData = volume.getItkData();

    for (int i = 0; i < 3; i++)
    {
        QCOMPARE(outputItkData->GetLargestPossibleRegion().GetSize()[i], itkData->GetLargestPossibleRegion().GetSize()[i]);
        QCOMPARE(outputItkData->GetLargestPossibleRegion().GetIndex()[i], itkData->GetLargestPossibleRegion().GetIndex()[i]);
        QCOMPARE(outputItkData->GetSpacing()[i], itkData->GetSpacing()[i]);
        QCOMPARE(outputItkData->GetOrigin()[i], itkData->GetOrigin()[i]);
    }

    itk::ImageRegionConstIterator<VolumePixelData::ItkImageType> actualIterator(outputItkData, outputItkData->GetLargestPossibleRegion());
    itk::ImageRegionConstIterator<VolumePixelData::ItkImageType> expectedIterator(itkData, itkData->GetLargestPossibleRegion());
    actualIterator.GoToBegin();
    expectedIterator.GoToBegin();

    while (!actualIterator.IsAtEnd())
    {
        // Optimization: using QCOMPARE in each loop is very slow in Qt5, so we compare values with a simple "if"
        // and just use QCOMPARE to fail the test and print the error when we know the values are different.
        if (actualIterator.Get() != expectedIterator.Get())
        {
            QCOMPARE(actualIterator.Get(), expectedIterator.Get());
        }

        ++actualIterator;
        ++expectedIterator;
    }
}

void test_Volume::setData_vtk_ShouldBehaveAsExpected_data()
{
    QTest::addColumn< vtkSmartPointer<vtkImageData> >("vtkData");
    QTest::addColumn<bool>("pixelDataLoaded");

    QTest::newRow("null") << vtkSmartPointer<vtkImageData>() << false;
    QTest::newRow("not null") << vtkSmartPointer<vtkImageData>::New() << true;
}

void test_Volume::setData_vtk_ShouldBehaveAsExpected()
{
    QFETCH(vtkSmartPointer<vtkImageData>, vtkData);
    QFETCH(bool, pixelDataLoaded);

    Volume volume;
    volume.setData(vtkData);

    QCOMPARE(volume.isPixelDataLoaded(), pixelDataLoaded);

    vtkImageData *outputVtkData = volume.getVtkData();
    QCOMPARE(outputVtkData, vtkData.GetPointer());
}

void test_Volume::setPixelData_ShouldBehaveAsExpected_data()
{
    QTest::addColumn<VolumePixelData*>("pixelData");
    QTest::addColumn<bool>("pixelDataLoaded");

    QTest::newRow("empty pixel data") << new VolumePixelData(this) << false;
    VolumePixelData *pixelData = new VolumePixelData(this);
    pixelData->setData(vtkImageData::New());
    QTest::newRow("\"filled\" pixel data") << pixelData << true;
}

void test_Volume::setPixelData_ShouldBehaveAsExpected()
{
    QFETCH(VolumePixelData*, pixelData);
    QFETCH(bool, pixelDataLoaded);

    Volume volume;
    volume.setPixelData(pixelData);

    QCOMPARE(volume.isPixelDataLoaded(), pixelDataLoaded);
    QCOMPARE(volume.getPixelData(), pixelData);
}

void test_Volume::getPixelData_ShouldReturnCurrentPixelData_data()
{
    QTest::addColumn<VolumePixelData*>("originalPixelData");
    QTest::addColumn<bool>("originalPixelDataLoaded");
    QTest::addColumn<VolumePixelData*>("returnedPixelData");
    QTest::addColumn<bool>("read");
    QTest::addColumn<bool>("returnedPixelDataLoaded");

    {
        VolumePixelData *readPixelData = new VolumePixelData(this);
        readPixelData->setData(vtkImageData::New());
        QTest::newRow("empty pixel data") << new VolumePixelData(this) << false << readPixelData << true << true;
    }

    {
        VolumePixelData *readPixelData = new VolumePixelData(this);
        readPixelData->setData(vtkImageData::New());
        VolumePixelData *pixelData = new VolumePixelData(this);
        pixelData->setData(vtkImageData::New());
        QTest::newRow("\"filled\" pixel data") << pixelData << true << pixelData << false << true;
    }
}

void test_Volume::getPixelData_ShouldReturnCurrentPixelData()
{
    QFETCH(VolumePixelData*, originalPixelData);
    QFETCH(bool, originalPixelDataLoaded);
    QFETCH(VolumePixelData*, returnedPixelData);
    QFETCH(bool, read);
    QFETCH(bool, returnedPixelDataLoaded);

    bool hasRead;
    TestingVolumeReader *volumeReader = new TestingVolumeReader(hasRead, this);
    volumeReader->m_readPixelData = returnedPixelData;
    TestingVolume volume;
    volume.m_volumeReaderToUse = volumeReader;
    volume.setPixelData(originalPixelData);

    QCOMPARE(volume.isPixelDataLoaded(), originalPixelDataLoaded);

    QCOMPARE(volume.getPixelData(), returnedPixelData);
    QCOMPARE(hasRead, read);
    QCOMPARE(volume.isPixelDataLoaded(), returnedPixelDataLoaded);
}

void test_Volume::getPixelData_ShouldRead()
{
    bool read;
    TestingVolumeReader *volumeReader = new TestingVolumeReader(read, this);
    TestingVolume volume;
    volume.m_volumeReaderToUse = volumeReader;

    QCOMPARE(volume.isPixelDataLoaded(), false);

    volume.getPixelData();

    QCOMPARE(read, true);
}

void test_Volume::getAcquisitionPlane_ShouldReturnNotAvailable_data()
{
    QTest::addColumn<QList<Image*> >("imageSet");

    QList<Image*> emptyImageList;
    QTest::newRow("emptyList") << emptyImageList;

    PatientOrientation patientOrientation;
    Image *image = new Image(this);
    image->setPatientOrientation(patientOrientation);
    QList<Image*> imageList;
    imageList << image;
    QTest::newRow("image with empty orientation") << imageList;
}

void test_Volume::getAcquisitionPlane_ShouldReturnNotAvailable()
{
    QFETCH(QList<Image*>, imageSet);

    Volume volume;
    volume.setImages(imageSet);
    
    QCOMPARE(volume.getAcquisitionPlane(), AnatomicalPlane(AnatomicalPlane::NotAvailable));
}

void test_Volume::getAcquisitionPlane_ShouldReturnExpectedPlane_data()
{
    QTest::addColumn<QList<Image*> >("imageSet");
    QTest::addColumn<AnatomicalPlane>("expectedResult");

    PatientOrientation axialPatientOrientation;
    axialPatientOrientation.setLabels("L", "P");

    PatientOrientation sagittalPatientOrientation;
    sagittalPatientOrientation.setLabels("P", "F");

    PatientOrientation coronalPatientOrientation;
    coronalPatientOrientation.setLabels("L", "F");
    
    PatientOrientation obliquePatientOrientation;
    obliquePatientOrientation.setLabels("L", "L");
    
    Image *axialImage = new Image(this);
    axialImage->setPatientOrientation(axialPatientOrientation);
    
    Image *sagittalImage = new Image(this);
    sagittalImage->setPatientOrientation(sagittalPatientOrientation);
    
    Image *coronalImage = new Image(this);
    coronalImage->setPatientOrientation(coronalPatientOrientation);
   
    Image *obliqueImage = new Image(this);
    obliqueImage->setPatientOrientation(obliquePatientOrientation);

    // Casos que retornen Axial
    QList<Image*> axialImages;
    axialImages << axialImage;
    QTest::newRow("Axial (1 image)") << axialImages << AnatomicalPlane(AnatomicalPlane::Axial);

    QList<Image*> axialImages2;
    axialImages2 << axialImage;
    axialImages2 << axialImage;
    QTest::newRow("Axial (2 images)") << axialImages2 << AnatomicalPlane(AnatomicalPlane::Axial);

    QList<Image*> axialImages2MixedA;
    axialImages2MixedA << axialImage;
    axialImages2MixedA << sagittalImage;
    QTest::newRow("Axial (2 images, 2nd is sagittal)") << axialImages2MixedA << AnatomicalPlane(AnatomicalPlane::Axial);

    QList<Image*> axialImages2MixedB;
    axialImages2MixedB << axialImage;
    axialImages2MixedB << coronalImage;
    QTest::newRow("Axial (2 images, 2nd is coronal)") << axialImages2MixedB << AnatomicalPlane(AnatomicalPlane::Axial);

    QList<Image*> axialImages2MixedC;
    axialImages2MixedB << axialImage;
    axialImages2MixedB << obliqueImage;
    QTest::newRow("Axial (2 images, 2nd is oblique)") << axialImages2MixedB << AnatomicalPlane(AnatomicalPlane::Axial);
    
    // Casos que retornen Sagittal
    QList<Image*> sagittalImages;
    sagittalImages << sagittalImage;
    QTest::newRow("Sagittal (1 image)") << sagittalImages << AnatomicalPlane(AnatomicalPlane::Sagittal);
    
    QList<Image*> sagittalImages2;
    sagittalImages2 << sagittalImage;
    sagittalImages2 << sagittalImage;
    QTest::newRow("Sagittal (2 images)") << sagittalImages2 << AnatomicalPlane(AnatomicalPlane::Sagittal);

    QList<Image*> sagittalImages2MixedA;
    sagittalImages2MixedA << sagittalImage;
    sagittalImages2MixedA << axialImage;
    QTest::newRow("Sagittal (2 images, 2nd is axial)") << sagittalImages2MixedA << AnatomicalPlane(AnatomicalPlane::Sagittal);

    QList<Image*> sagittalImages2MixedB;
    sagittalImages2MixedB << sagittalImage;
    sagittalImages2MixedB << coronalImage;
    QTest::newRow("Sagittal (2 images, 2nd is coronal)") << sagittalImages2MixedB << AnatomicalPlane(AnatomicalPlane::Sagittal);

    QList<Image*> sagittalImages2MixedC;
    sagittalImages2MixedB << sagittalImage;
    sagittalImages2MixedB << obliqueImage;
    QTest::newRow("Sagittal (2 images, 2nd is oblique)") << sagittalImages2MixedB << AnatomicalPlane(AnatomicalPlane::Sagittal);
    
    // Casos que retornen Coronal
    QList<Image*> coronalImages;
    coronalImages << coronalImage;
    QTest::newRow("Coronal (1 image)") << coronalImages << AnatomicalPlane(AnatomicalPlane::Coronal);

    QList<Image*> coronalImages2;
    coronalImages2 << coronalImage;
    coronalImages2 << coronalImage;
    QTest::newRow("Coronal (2 images)") << coronalImages2 << AnatomicalPlane(AnatomicalPlane::Coronal);

    QList<Image*> coronalImages2MixedA;
    coronalImages2MixedA << coronalImage;
    coronalImages2MixedA << sagittalImage;
    QTest::newRow("Coronal (2 images, 2nd is sagittal)") << coronalImages2MixedA << AnatomicalPlane(AnatomicalPlane::Coronal);

    QList<Image*> coronalImages2MixedB;
    coronalImages2MixedB << coronalImage;
    coronalImages2MixedB << axialImage;
    QTest::newRow("Coronal (2 images, 2nd is axial)") << coronalImages2MixedB << AnatomicalPlane(AnatomicalPlane::Coronal);
    
    QList<Image*> coronalImages2MixedC;
    coronalImages2MixedB << coronalImage;
    coronalImages2MixedB << obliqueImage;
    QTest::newRow("Coronal (2 images, 2nd is oblique)") << coronalImages2MixedB << AnatomicalPlane(AnatomicalPlane::Coronal);

    // Casos que retornen Oblique
    QList<Image*> obliqueImages;
    obliqueImages << obliqueImage;
    QTest::newRow("Oblique (1 image)") << obliqueImages << AnatomicalPlane(AnatomicalPlane::Oblique);

    QList<Image*> obliqueImages2;
    obliqueImages2 << obliqueImage;
    obliqueImages2 << obliqueImage;
    QTest::newRow("Oblique (2 images)") << obliqueImages2 << AnatomicalPlane(AnatomicalPlane::Oblique);

    QList<Image*> obliqueImages2MixedA;
    obliqueImages2MixedA << obliqueImage;
    obliqueImages2MixedA << sagittalImage;
    QTest::newRow("Oblique (2 images, 2nd is sagittal)") << obliqueImages2MixedA << AnatomicalPlane(AnatomicalPlane::Oblique);

    QList<Image*> obliqueImages2MixedB;
    obliqueImages2MixedB << obliqueImage;
    obliqueImages2MixedB << axialImage;
    QTest::newRow("Oblique (2 images, 2nd is axial)") << obliqueImages2MixedB << AnatomicalPlane(AnatomicalPlane::Oblique);

    QList<Image*> obliqueImages2MixedC;
    obliqueImages2MixedC << obliqueImage;
    obliqueImages2MixedC << coronalImage;
    QTest::newRow("Oblique (2 images, 2nd is coronal)") << obliqueImages2MixedC << AnatomicalPlane(AnatomicalPlane::Oblique);
}

void test_Volume::getAcquisitionPlane_ShouldReturnExpectedPlane()
{
    QFETCH(QList<Image*>, imageSet);
    QFETCH(AnatomicalPlane, expectedResult);

    Volume volume;
    volume.setImages(imageSet);
    
    QCOMPARE(volume.getAcquisitionPlane(), expectedResult);
}

void test_Volume::getCorrespondingOrthogonalPlane_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<AnatomicalPlane>("anatomicalPlane");
    QTest::addColumn<OrthogonalPlane>("expectedOrthogonalPlane");

    PatientOrientation axialPatientOrientation = AnatomicalPlane(AnatomicalPlane::Axial).getDefaultRadiologicalOrientation();
    Image *axialImage = new Image(this);
    axialImage->setPatientOrientation(axialPatientOrientation);
    Volume *axialVolume = new Volume(this);
    axialVolume->addImage(axialImage);

    QTest::newRow("Axial volume, axial plane") << axialVolume << AnatomicalPlane(AnatomicalPlane::Axial) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Axial volume, sagittal plane") << axialVolume << AnatomicalPlane(AnatomicalPlane::Sagittal) << OrthogonalPlane(OrthogonalPlane::YZPlane);
    QTest::newRow("Axial volume, coronal plane") << axialVolume << AnatomicalPlane(AnatomicalPlane::Coronal) << OrthogonalPlane(OrthogonalPlane::XZPlane);
    QTest::newRow("Axial volume, N/A plane") << axialVolume << AnatomicalPlane(AnatomicalPlane::NotAvailable) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Axial volume, Oblique plane") << axialVolume << AnatomicalPlane(AnatomicalPlane::Oblique) << OrthogonalPlane(OrthogonalPlane::XYPlane);

    PatientOrientation sagittalPatientOrientation = AnatomicalPlane(AnatomicalPlane::Sagittal).getDefaultRadiologicalOrientation();
    Image *sagittalImage = new Image(this);
    sagittalImage->setPatientOrientation(sagittalPatientOrientation);
    Volume *sagittalVolume = new Volume(this);
    sagittalVolume->addImage(sagittalImage);

    QTest::newRow("Sagittal volume, axial plane") << sagittalVolume << AnatomicalPlane(AnatomicalPlane::Axial) << OrthogonalPlane(OrthogonalPlane::XZPlane);
    QTest::newRow("Sagittal volume, sagittal plane") << sagittalVolume << AnatomicalPlane(AnatomicalPlane::Sagittal) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Sagittal volume, coronal plane") << sagittalVolume << AnatomicalPlane(AnatomicalPlane::Coronal) << OrthogonalPlane(OrthogonalPlane::YZPlane);
    QTest::newRow("Sagittal volume, N/A plane") << sagittalVolume << AnatomicalPlane(AnatomicalPlane::NotAvailable) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Sagittal volume, Oblique plane") << sagittalVolume << AnatomicalPlane(AnatomicalPlane::Oblique) << OrthogonalPlane(OrthogonalPlane::XYPlane);

    PatientOrientation coronalPatientOrientation = AnatomicalPlane(AnatomicalPlane::Coronal).getDefaultRadiologicalOrientation();
    Image *coronalImage = new Image(this);
    coronalImage->setPatientOrientation(coronalPatientOrientation);
    Volume *coronalVolume = new Volume(this);
    coronalVolume->addImage(coronalImage);

    QTest::newRow("Coronal volume, axial plane") << coronalVolume << AnatomicalPlane(AnatomicalPlane::Axial) << OrthogonalPlane(OrthogonalPlane::XZPlane);
    QTest::newRow("Coronal volume, sagittal plane") << coronalVolume << AnatomicalPlane(AnatomicalPlane::Sagittal) << OrthogonalPlane(OrthogonalPlane::YZPlane);
    QTest::newRow("Coronal volume, coronal plane") << coronalVolume << AnatomicalPlane(AnatomicalPlane::Coronal) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Coronal volume, N/A plane") << coronalVolume << AnatomicalPlane(AnatomicalPlane::NotAvailable) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Coronal volume, Oblique plane") << coronalVolume << AnatomicalPlane(AnatomicalPlane::Oblique) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    
    PatientOrientation obliquePatientOrientation;
    obliquePatientOrientation.setLabels("L", "L");
    Image *obliqueImage = new Image(this);
    obliqueImage->setPatientOrientation(obliquePatientOrientation);
    Volume *obliqueVolume = new Volume(this);
    obliqueVolume->addImage(obliqueImage);

    QTest::newRow("Oblique volume, axial plane") << obliqueVolume << AnatomicalPlane(AnatomicalPlane::Axial) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Oblique volume, sagittal plane") << obliqueVolume << AnatomicalPlane(AnatomicalPlane::Sagittal) << OrthogonalPlane(OrthogonalPlane::YZPlane);
    QTest::newRow("Oblique volume, coronal plane") << obliqueVolume << AnatomicalPlane(AnatomicalPlane::Coronal) << OrthogonalPlane(OrthogonalPlane::XZPlane);
    QTest::newRow("Oblique volume, N/A plane") << obliqueVolume << AnatomicalPlane(AnatomicalPlane::NotAvailable) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Oblique volume, Oblique plane") << obliqueVolume << AnatomicalPlane(AnatomicalPlane::Oblique) << OrthogonalPlane(OrthogonalPlane::XYPlane);

    Image *naImage = new Image(this);
    Volume *naVolume = new Volume(this);
    naVolume->addImage(naImage);

    QTest::newRow("Not Available Plane volume, axial plane") << naVolume << AnatomicalPlane(AnatomicalPlane::Axial) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Not Available Plane volume, sagittal plane") << naVolume << AnatomicalPlane(AnatomicalPlane::Sagittal) << OrthogonalPlane(OrthogonalPlane::YZPlane);
    QTest::newRow("Not Available Plane volume, coronal plane") << naVolume << AnatomicalPlane(AnatomicalPlane::Coronal) << OrthogonalPlane(OrthogonalPlane::XZPlane);
    QTest::newRow("Not Available Plane volume, N/A plane") << naVolume << AnatomicalPlane(AnatomicalPlane::NotAvailable) << OrthogonalPlane(OrthogonalPlane::XYPlane);
    QTest::newRow("Not Available Plane volume, Oblique plane") << naVolume << AnatomicalPlane(AnatomicalPlane::Oblique) << OrthogonalPlane(OrthogonalPlane::XYPlane);
}

void test_Volume::getCorrespondingOrthogonalPlane_ShouldReturnExpectedValues()
{
    QFETCH(Volume*, volume);
    QFETCH(AnatomicalPlane, anatomicalPlane);
    QFETCH(OrthogonalPlane, expectedOrthogonalPlane);
    
    QCOMPARE(volume->getCorrespondingOrthogonalPlane(anatomicalPlane), expectedOrthogonalPlane);
}

void test_Volume::getStackDirection_ShouldNotModifyDirection_data()
{
    QTest::addColumn<QVector3D>("direction");

    QTest::newRow("(0,0,0)") << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("(1,2,3)") << QVector3D(1.0, 2.0, 3.0);
}

void test_Volume::getStackDirection_ShouldNotModifyDirection()
{
    QFETCH(QVector3D, direction);

    Volume volume;
    double dir[3] = { direction.x(), direction.y(), direction.z() };
    volume.getStackDirection(dir);
    QVector3D newDirection(dir[0], dir[1], dir[2]);

    QCOMPARE(newDirection, direction);
}

void test_Volume::getStackDirection_ShouldReturnNormalVector_data()
{
    QTest::addColumn<QSharedPointer<Volume> >("volume");
    QTest::addColumn<ImageOrientation>("imageOrientation");
    QTest::addColumn<Vector3>("normal");

    QSharedPointer<Volume> volume(new Volume());
    Image *image1 = new Image(volume.data());
    volume->addImage(image1);
    ImageOrientation imageOrientation;
    Vector3 positiveX(+1.0, 0.0, 0.0);
    Vector3 negativeX(-1.0, 0.0, 0.0);
    Vector3 positiveY(0.0, +1.0, 0.0);
    Vector3 negativeY(0.0, -1.0, 0.0);
    Vector3 positiveZ(0.0, 0.0, +1.0);
    Vector3 negativeZ(0.0, 0.0, -1.0);

    QTest::newRow("default orientation") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(positiveX, positiveY);
    QTest::newRow("+x +y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveX, negativeY);
    QTest::newRow("+x -y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveX, positiveZ);
    QTest::newRow("+x +z") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveX, negativeZ);
    QTest::newRow("+x -z") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(negativeX, positiveY);
    QTest::newRow("-x +y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeX, negativeY);
    QTest::newRow("-x -y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeX, positiveZ);
    QTest::newRow("-x +z") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeX, negativeZ);
    QTest::newRow("-x -z") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(positiveY, positiveX);
    QTest::newRow("+y +x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveY, negativeX);
    QTest::newRow("+y -x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveY, positiveZ);
    QTest::newRow("+y +z") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveY, negativeZ);
    QTest::newRow("+y -z") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(negativeY, positiveX);
    QTest::newRow("-y +x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeY, negativeX);
    QTest::newRow("-y -x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeY, positiveZ);
    QTest::newRow("-y +z") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeY, negativeZ);
    QTest::newRow("-y -z") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(positiveZ, positiveX);
    QTest::newRow("+z +x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveZ, negativeX);
    QTest::newRow("+z -x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveZ, positiveY);
    QTest::newRow("+z +y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(positiveZ, negativeY);
    QTest::newRow("+z -y") << volume << imageOrientation << imageOrientation.getNormalVector();

    imageOrientation.setRowAndColumnVectors(negativeZ, positiveX);
    QTest::newRow("-z +x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeZ, negativeX);
    QTest::newRow("-z -x") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeZ, positiveY);
    QTest::newRow("-z +y") << volume << imageOrientation << imageOrientation.getNormalVector();
    imageOrientation.setRowAndColumnVectors(negativeZ, negativeY);
    QTest::newRow("-z -y") << volume << imageOrientation << imageOrientation.getNormalVector();

    Vector3 row, column;

    row.x = +3.9;
    row.y = +9.5;
    row.z = +0.8;
    column.x = -6.0;
    column.y = -6.7;
    column.z = -3.2;
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (1)") << volume << imageOrientation << imageOrientation.getNormalVector();

    row.x = +1.1;
    row.y = +8.0;
    row.z = -4.7;
    column.x = +3.5;
    column.y = -8.4;
    column.z = +6.7;
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (2)") << volume << imageOrientation << imageOrientation.getNormalVector();

    row.x = +3.0;
    row.y = -0.6;
    row.z = +1.0;
    column.x = -8.6;
    column.y = +3.4;
    column.z = +7.2;
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (3)") << volume << imageOrientation << imageOrientation.getNormalVector();
}

void test_Volume::getStackDirection_ShouldReturnNormalVector()
{
    QFETCH(QSharedPointer<Volume>, volume);
    QFETCH(ImageOrientation, imageOrientation);
    QFETCH(Vector3, normal);

    volume->getImage(0)->setImageOrientationPatient(imageOrientation);
    Vector3 direction;
    volume->getStackDirection(direction.data());

    QCOMPARE(direction, normal);
}

void test_Volume::getStackDirection_ShouldReturnExpectedDirection_data()
{
    QTest::addColumn<QSharedPointer<Volume> >("volume");
    QTest::addColumn<QVector3D>("position1");
    QTest::addColumn<QVector3D>("position2");
    QTest::addColumn<QVector3D>("expectedDirection");

    QSharedPointer<Volume> volume(new Volume());
    Image *image1 = new Image(volume.data());
    Image *image2 = new Image(volume.data());
    volume->setImages(QList<Image*>() << image1 << image2);

    QTest::newRow("+x (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+1.0, 0.0, 0.0) << QVector3D(+1.0, 0.0, 0.0);
    QTest::newRow("-x (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(-1.0, 0.0, 0.0) << QVector3D(-1.0, 0.0, 0.0);
    QTest::newRow("+y (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, +1.0, 0.0) << QVector3D(0.0, +1.0, 0.0);
    QTest::newRow("-y (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, -1.0, 0.0) << QVector3D(0.0, -1.0, 0.0);
    QTest::newRow("+z (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, +1.0) << QVector3D(0.0, 0.0, +1.0);
    QTest::newRow("-z (distance 1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, -1.0) << QVector3D(0.0, 0.0, -1.0);
    
    QTest::newRow("+x (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+10.0, 0.0, 0.0) << QVector3D(+1.0, 0.0, 0.0);
    QTest::newRow("-x (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(-10.0, 0.0, 0.0) << QVector3D(-1.0, 0.0, 0.0);
    QTest::newRow("+y (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, +10.0, 0.0) << QVector3D(0.0, +1.0, 0.0);
    QTest::newRow("-y (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, -10.0, 0.0) << QVector3D(0.0, -1.0, 0.0);
    QTest::newRow("+z (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, +10.0) << QVector3D(0.0, 0.0, +1.0);
    QTest::newRow("-z (distance 10 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, -10.0) << QVector3D(0.0, 0.0, -1.0);

    QTest::newRow("+x (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+0.1, 0.0, 0.0) << QVector3D(+1.0, 0.0, 0.0);
    QTest::newRow("-x (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(-0.1, 0.0, 0.0) << QVector3D(-1.0, 0.0, 0.0);
    QTest::newRow("+y (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, +0.1, 0.0) << QVector3D(0.0, +1.0, 0.0);
    QTest::newRow("-y (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, -0.1, 0.0) << QVector3D(0.0, -1.0, 0.0);
    QTest::newRow("+z (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, +0.1) << QVector3D(0.0, 0.0, +1.0);
    QTest::newRow("-z (distance 0.1 between planes)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, -0.1) << QVector3D(0.0, 0.0, -1.0);

    QTest::newRow("oblique (1)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+2.3, +6.5, -1.7) << QVector3D(+0.3238794, +0.9153114, -0.2393891);
    QTest::newRow("oblique (2)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+3.0, -2.4, -3.3) << QVector3D(+0.5923488, -0.4738791, -0.6515837);
    QTest::newRow("oblique (3)") << volume << QVector3D(0.0, 0.0, 0.0) << QVector3D(+8.5, +5.2, -4.8) << QVector3D(+0.7685151, +0.4701504, -0.4339850);
}

void test_Volume::getStackDirection_ShouldReturnExpectedDirection()
{
    QFETCH(QSharedPointer<Volume>, volume);
    QFETCH(QVector3D, position1);
    QFETCH(QVector3D, position2);
    QFETCH(QVector3D, expectedDirection);

    double pos1[3] = { position1.x(), position1.y(), position1.z() };
    double pos2[3] = { position2.x(), position2.y(), position2.z() };
    volume->getImage(0)->setImagePositionPatient(pos1);
    volume->getImage(1)->setImagePositionPatient(pos2);
    double dir[3];
    volume->getStackDirection(dir);
    QVector3D direction(dir[0], dir[1], dir[2]);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(direction, expectedDirection, 0.0000001));
}

void test_Volume::getPhaseImages_ShouldReturnExpectedPhaseImages_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("phase");
    QTest::addColumn< QList<Image*> >("phaseImages");

    Volume *volumeWithPhases = VolumeTestHelper::createVolume(6, 2, 3);
    QList<Image*> images_1 = volumeWithPhases->getImages();

    Volume *volumeWithoutPhases = VolumeTestHelper::createVolume(2, 1, 2);
    QList<Image*> images_2 = volumeWithoutPhases->getImages();

    Volume *volumeGeneric = VolumeTestHelper::createVolume(2, 1, 2);

    QTest::newRow("Volume with phases") << volumeWithPhases << 1 << (QList<Image*>() << images_1.at(1) << images_1.at(3) << images_1.at(5));
    QTest::newRow("Volume without phases") << volumeWithoutPhases << 0 << (QList<Image*>() << images_2.at(0) << images_2.at(1));
    QTest::newRow("Incorrect phase") << volumeGeneric << -1 << QList<Image*>();
}

void test_Volume::getPhaseImages_ShouldReturnExpectedPhaseImages()
{
    QFETCH(Volume*, volume);
    QFETCH(int, phase);
    QFETCH(QList<Image*>, phaseImages);

    QCOMPARE(volume->getPhaseImages(phase).size(), phaseImages.size());
    QCOMPARE(phaseImages, volume->getPhaseImages(phase));

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getOrigin_ShouldReturnExpectedOrigin_data()
{
     QTest::addColumn<Volume*>("volume");
     QTest::addColumn<double>("xValue");
     QTest::addColumn<double>("yValue");
     QTest::addColumn<double>("zValue");

     double spacing[3];
     int extent[6];
     double origin[3];
     origin[0] = 10.56;
     origin[1] = -45.185;
     origin[2] = 12;
     Volume *volumeWithOrigin = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent);

     double nullOrigin[3];
     nullOrigin[0] = NULL;
     nullOrigin[1] = NULL;
     nullOrigin[2] = NULL;
     Volume *volumeWithNullOrigin = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, nullOrigin, spacing, extent);

     QTest::newRow("Volume with origin") << volumeWithOrigin << origin[0] << origin[1] << origin[2];
     QTest::newRow("Volume with null origin") << volumeWithNullOrigin << 0.0 << 0.0 << 0.0;
}

void test_Volume::getOrigin_ShouldReturnExpectedOrigin()
{
    QFETCH(Volume*, volume);
    QFETCH(double, xValue);
    QFETCH(double, yValue);
    QFETCH(double, zValue);

    QCOMPARE(volume->getOrigin()[0], xValue);
    QCOMPARE(volume->getOrigin()[1], yValue);
    QCOMPARE(volume->getOrigin()[2], zValue);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getDimensions_ShouldReturnExpectedDimensions_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("xValue");
    QTest::addColumn<int>("yValue");
    QTest::addColumn<int>("zValue");

    double origin[3];
    double spacing[3];
    int extent[6] = { 0, 127, 0, 511, 0, 44 };
    Volume *volumeWithDimensions = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent);
    QTest::newRow("Volume with dimensions") << volumeWithDimensions << 128 << 512 << 45;

    extent[0] = extent[2] = extent[4] = 0;
    extent[1] = extent[3] = extent[5] = -1;
    Volume *volumeWithNullDimensions = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent);
    QTest::newRow("Volume with null dimensions") << volumeWithNullDimensions << 0 << 0 << 0;
}

void test_Volume::getDimensions_ShouldReturnExpectedDimensions()
{
    QFETCH(Volume*, volume);
    QFETCH(int, xValue);
    QFETCH(int, yValue);
    QFETCH(int, zValue);

    QCOMPARE(volume->getDimensions()[0], xValue);
    QCOMPARE(volume->getDimensions()[1], yValue);
    QCOMPARE(volume->getDimensions()[2], zValue);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getCorners_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<Corners>("expectedCorners");

    double origin[3] = { 12, 8, 4 };
    double spacing[3] = { 0.3, 0.8, 1.5 };
    int extent[6] = { 10, 20, -9, 2, 0, 38 };
    double xMin = 15, xMax = 18, yMin = 0.8, yMax = 9.6, zMin = 4, zMax = 61;
    QTest::newRow("3d volume") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                               << Corners{{Vector3(xMin, yMin, zMin), Vector3(xMax, yMin, zMin), Vector3(xMin, yMax, zMin), Vector3(xMax, yMax, zMin),
                                           Vector3(xMin, yMin, zMax), Vector3(xMax, yMin, zMax), Vector3(xMin, yMax, zMax), Vector3(xMax, yMax, zMax)}};

    zMax = 22;
    QTest::newRow("3d volume with phases") << VolumeTestHelper::createVolumeWithParameters(39, 3, 13, origin, spacing, extent)
                                           << Corners{{Vector3(xMin, yMin, zMin), Vector3(xMax, yMin, zMin),
                                                       Vector3(xMin, yMax, zMin), Vector3(xMax, yMax, zMin),
                                                       Vector3(xMin, yMin, zMax), Vector3(xMax, yMin, zMax),
                                                       Vector3(xMin, yMax, zMax), Vector3(xMax, yMax, zMax)}};

    extent[5] = 0;
    zMax = 4;
    QTest::newRow("single slice volume") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                                         << Corners{{Vector3(xMin, yMin, zMin), Vector3(xMax, yMin, zMin),
                                                     Vector3(xMin, yMax, zMin), Vector3(xMax, yMax, zMin),
                                                     Vector3(xMin, yMin, zMax), Vector3(xMax, yMin, zMax),
                                                     Vector3(xMin, yMax, zMax), Vector3(xMax, yMax, zMax)}};

    extent[5] = 2;
    QTest::newRow("single slice volume with phases") << VolumeTestHelper::createVolumeWithParameters(3, 3, 1, origin, spacing, extent)
                                                     << Corners{{Vector3(xMin, yMin, zMin), Vector3(xMax, yMin, zMin),
                                                                 Vector3(xMin, yMax, zMin), Vector3(xMax, yMax, zMin),
                                                                 Vector3(xMin, yMin, zMax), Vector3(xMax, yMin, zMax),
                                                                 Vector3(xMin, yMax, zMax), Vector3(xMax, yMax, zMax)}};
}

void test_Volume::getCorners_ShouldReturnExpectedValues()
{
    QFETCH(Volume*, volume);
    QFETCH(Corners, expectedCorners);

    Corners corners = volume->getCorners();

    for (int i = 0; i < 8; i++)
    {
        QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(corners[i], expectedCorners[i]));
    }

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getSpacing_ShouldReturnExpectedSpacing_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("xValue");
    QTest::addColumn<double>("yValue");
    QTest::addColumn<double>("zValue");

    double origin[3];
    int extent[6];
    double spacing[3];
    spacing[0] = 3;
    spacing[1] = -2;
    spacing[2] = 2.5;
    Volume *volumeWithSpacing = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent);

    double nullSpacing[3];
    nullSpacing[0] = NULL;
    nullSpacing[1] = NULL;
    nullSpacing[2] = NULL;
    Volume *volumeWithNullSpacing = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, nullSpacing, extent);

    QTest::newRow("Volume with spacing") << volumeWithSpacing << spacing[0] << spacing[1] << spacing[2];
    QTest::newRow("Volume with null spacing") << volumeWithNullSpacing << 0.0 << 0.0 << 0.0;
}

void test_Volume::getSpacing_ShouldReturnExpectedSpacing()
{
    QFETCH(Volume*, volume);
    QFETCH(double, xValue);
    QFETCH(double, yValue);
    QFETCH(double, zValue);

    QCOMPARE(volume->getSpacing()[0], xValue);
    QCOMPARE(volume->getSpacing()[1], yValue);
    QCOMPARE(volume->getSpacing()[2], zValue);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getExtent_ShouldReturnExpectedExtent_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("x1");
    QTest::addColumn<int>("x2");
    QTest::addColumn<int>("y1");
    QTest::addColumn<int>("y2");
    QTest::addColumn<int>("z1");
    QTest::addColumn<int>("z2");

    double origin[3];
    double spacing[3];
    int extent[6];
    extent[0] = 10;
    extent[1] = -2;
    extent[2] = 9;
    extent[3] = 2;
    extent[4] = 100;
    extent[5] = -500;

    Volume *volumeWithExtent = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent);

    int nullExtent[6];
    nullExtent[0] = NULL;
    nullExtent[1] = NULL;
    nullExtent[2] = NULL;
    nullExtent[3] = NULL;
    nullExtent[4] = NULL;
    nullExtent[5] = NULL;
    
    Volume *volumeWithNullExtent = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, nullExtent);

    QTest::newRow("Volume with extent") << volumeWithExtent << extent[0] << extent[1] << extent[2] << extent[3] << extent[4] << extent[5];
    QTest::newRow("Volume with null extent") << volumeWithNullExtent << 0 << 0 << 0 << 0 << 0 << 0;
}

void test_Volume::getExtent_ShouldReturnExpectedExtent()
{
    QFETCH(Volume*, volume);
    QFETCH(int, x1);
    QFETCH(int, x2);
    QFETCH(int, y1);
    QFETCH(int, y2);
    QFETCH(int, z1);
    QFETCH(int, z2);

    QCOMPARE(volume->getExtent()[0], x1);
    QCOMPARE(volume->getExtent()[1], x2);
    QCOMPARE(volume->getExtent()[2], y1);
    QCOMPARE(volume->getExtent()[3], y2);
    QCOMPARE(volume->getExtent()[4], z1);
    QCOMPARE(volume->getExtent()[5], z2);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::setPhases_ShouldSetValidPhasesValue_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("phases");

    Volume *volumeWithValidPhases = VolumeTestHelper::createVolume();
    volumeWithValidPhases->setNumberOfPhases(10);

    Volume *volumeWithInvalidPhases = VolumeTestHelper::createVolume();
    volumeWithInvalidPhases->setNumberOfPhases(-5);

    Volume *volumeWithZeroPhases = VolumeTestHelper::createVolume();
    volumeWithZeroPhases->setNumberOfPhases(0);

    QTest::newRow("Volume with valid phases") << volumeWithValidPhases << 10;
    QTest::newRow("Volume with invalid phases") << volumeWithInvalidPhases << 1;
    QTest::newRow("Volume with zero phases") << volumeWithZeroPhases << 1;
}

void test_Volume::setPhases_ShouldSetValidPhasesValue()
{
    QFETCH(Volume*, volume);
    QFETCH(int, phases);

    QCOMPARE(volume->getNumberOfPhases(), phases);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::addImage_ShouldAddValidImages_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn< QList<Image*> >("images");
    QTest::addColumn<bool>("loaded");

    Volume *volumeWithTwoDifferentImages = VolumeTestHelper::createVolume();
    Image *image_1 = ImageTestHelper::createImageByUID("1");
    Image *image_2 = ImageTestHelper::createImageByUID("2");
    volumeWithTwoDifferentImages->addImage(image_1);
    volumeWithTwoDifferentImages->addImage(image_2);

    Volume *volumeWithDupplicate = VolumeTestHelper::createVolume(1);
    volumeWithDupplicate->addImage(volumeWithDupplicate->getImages().at(0));

    QTest::newRow("Volume with two different images") << volumeWithTwoDifferentImages << (QList<Image*>() << image_1 << image_2) << false;
    QTest::newRow("Volume with dipplicate") << volumeWithDupplicate << (QList<Image*>() << volumeWithDupplicate->getImages().at(0)) << true;
}

void test_Volume::addImage_ShouldAddValidImages()
{
    QFETCH(Volume*, volume);
    QFETCH(QList<Image*>, images);
    QFETCH(bool, loaded);

    QCOMPARE(volume->getImages().size(), images.size());
    QCOMPARE(volume->getImages(), images);
    QCOMPARE(volume->isPixelDataLoaded(), loaded);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::setImages_ShouldAddImageList_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn< QList<Image*> >("images");
    QTest::addColumn<bool>("loaded");


    Volume *volumeWithImageList = VolumeTestHelper::createVolume();
    QList<Image*> imageList_1;
    QList<Image*> imageList_2;
    Image *image_1 = ImageTestHelper::createImageByUID("1");
    Image *image_2 = ImageTestHelper::createImageByUID("2");
    Image *image_3 = ImageTestHelper::createImageByUID("3");
    imageList_1 << image_1 << image_2;
    imageList_2 << image_3;
    volumeWithImageList->setImages(imageList_1);
    volumeWithImageList->setImages(imageList_2);

    Volume *volumeWithNoImages = VolumeTestHelper::createVolume();
    Image *image = ImageTestHelper::createImageByUID("1");
    volumeWithNoImages->addImage(image);
    QList<Image*> imageList_3;
    volumeWithNoImages->setImages(imageList_3);

    QTest::newRow("Volume with two different images") << volumeWithImageList << imageList_2 << false;
    QTest::newRow("Volume with dipplicate") << volumeWithNoImages << imageList_3 << false;
}

void test_Volume::setImages_ShouldAddImageList()
{
    QFETCH(Volume*, volume);
    QFETCH(QList<Image*>, images);
    QFETCH(bool, loaded);

    QCOMPARE(volume->getImages().size(), images.size());
    QCOMPARE(volume->getImages(), images);
    QCOMPARE(volume->isPixelDataLoaded(), loaded);
    
    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::convertToNeutralVolume_ShouldBehaveAsExpected_data()
{
    QTest::addColumn<Volume*>("volume");

    {
        QTest::newRow("default") << new Volume(this);
    }

    {
        int dimensions[3] = { 200, 200, 200 };
        int extent[6] = { 0, 199, 0, 199, 0, 199 };
        double spacing[3] = { 0.33, 0.33, 1.20 };
        double origin[3] = { 0.0, 0.0, 0.0 };
        VolumePixelData *pixelData = VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
        Volume *volume = new Volume(this);
        volume->setPixelData(pixelData);
        QTest::newRow("random #1") << volume;
    }

    {
        int dimensions[3] = { 33, 124, 6 };
        int extent[6] = { 200, 232, 169, 292, 156, 161 };
        double spacing[3] = { 2.2, 0.74, 1.44 };
        double origin[3] = { 48.0, 41.0, -68.0 };
        VolumePixelData *pixelData = VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
        Volume *volume = new Volume(this);
        volume->setPixelData(pixelData);
        volume->setNumberOfPhases(42);
        QTest::newRow("random #2") << volume;
    }
}

void test_Volume::convertToNeutralVolume_ShouldBehaveAsExpected()
{
    QFETCH(Volume*, volume);

    volume->convertToNeutralVolume();

    VolumePixelData *pixelData = volume->getPixelData();
    QVERIFY(pixelData != 0);

    vtkImageData *vtkData = pixelData->getVtkData();
    QVERIFY(vtkData != 0);

    for (int i = 0; i < 3; i++)
    {
        QCOMPARE(vtkData->GetOrigin()[i], 0.0);
        QCOMPARE(vtkData->GetSpacing()[i], 1.0);
    }

    QCOMPARE(vtkData->GetDimensions()[0], 10);
    QCOMPARE(vtkData->GetDimensions()[1], 10);
    QCOMPARE(vtkData->GetDimensions()[2], 1);

    QCOMPARE(vtkData->GetExtent()[0], 0);
    QCOMPARE(vtkData->GetExtent()[1], 9);
    QCOMPARE(vtkData->GetExtent()[2], 0);
    QCOMPARE(vtkData->GetExtent()[3], 9);
    QCOMPARE(vtkData->GetExtent()[4], 0);
    QCOMPARE(vtkData->GetExtent()[5], 0);

    QCOMPARE(vtkData->GetScalarType(), VTK_SHORT);
    QCOMPARE(vtkData->GetNumberOfScalarComponents(), 1);

    QVERIFY(vtkData->GetScalarPointer() != 0);

    signed short *scalarPointer = static_cast<signed short*>(vtkData->GetScalarPointer());
    signed short expectedValue;

    for (int i = 0; i < 10; i++)
    {
        expectedValue = 150 - i * 20;

        if (i > 4)
        {
            expectedValue = 150 - (10 - i - 1) * 20;
        }

        for (int j = 0; j < 10; j++)
        {
            QCOMPARE(*scalarPointer, expectedValue);
            scalarPointer++;
        }
    }

    QCOMPARE(volume->getNumberOfPhases(), 1);
    QCOMPARE(volume->isPixelDataLoaded(), true);
}

void test_Volume::getImage_ShouldReturnExpectedImage_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("sliceNumber");
    QTest::addColumn<int>("phaseNumber");
    QTest::addColumn<Image*>("image");

    Volume *volumeWithoutPhases = VolumeTestHelper::createVolume(5);
    Volume *volumeWithPhases = VolumeTestHelper::createVolume(6, 2, 3);
    Volume *volumeGeneric = VolumeTestHelper::createVolume(2);
    Image *image = NULL;

    QTest::newRow("Volume with 1 phase") << volumeWithoutPhases << 0 << 0 << volumeWithoutPhases->getImages().at(0);
    QTest::newRow("Volume with phases") << volumeWithPhases << 2 << 1 << volumeWithPhases->getImages().at(5);
    QTest::newRow("Incorrect image") << volumeGeneric << -1 << -1 << image;
}

void test_Volume::getImage_ShouldReturnExpectedImage()
{
    QFETCH(Volume*, volume);
    QFETCH(int, sliceNumber);
    QFETCH(int, phaseNumber);
    QFETCH(Image*, image);

    QCOMPARE(volume->getImage(sliceNumber, phaseNumber), image);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::getPixelUnits_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QString>("modality");
    QTest::addColumn<QString>("expectedUnits");

    QTest::newRow("CT, Hounsfield units") << "CT" << "HU";

    QStringList unspecifiedUnitsModalities;
    unspecifiedUnitsModalities << "CR" << "MR" << "PT" << "ES" << "XA" << "US" << "XC" << "SC" << "OT" << "MG" << "RF";

    foreach (const QString &modality, unspecifiedUnitsModalities)
    {
        QTest::newRow(qPrintable(QString("%1, unspecified units").arg(modality))) << modality << QString();
    }
}

void test_Volume::getPixelUnits_ShouldReturnExpectedValue()
{
    QFETCH(QString, modality);
    QFETCH(QString, expectedUnits);
    
    Series *series = SeriesTestHelper::createSeriesByModality(modality, 1);
    Volume *volume = new Volume(this);
    volume->setImages(series->getImages());

    QCOMPARE(volume->getPixelUnits(), expectedUnits);
    
    SeriesTestHelper::cleanUp(series);
}

void test_Volume::getImageIndex_ShouldReturnExpectedImageIndex_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<int>("sliceNumber");
    QTest::addColumn<int>("phaseNumber");
    QTest::addColumn<int>("result");

    Volume *volumeWithoutPhases = VolumeTestHelper::createVolume(5);
    Volume *volumeWithPhases = VolumeTestHelper::createVolume(6, 2, 3);
    Volume *volumeGeneric_1 = VolumeTestHelper::createVolume(2);
    Volume *volumeGeneric_2 = VolumeTestHelper::createVolume(2);

    QTest::newRow("Volume with 1 phase") << volumeWithoutPhases << 0 << 0 << 0;
    QTest::newRow("Volume with phases") << volumeWithPhases << 2 << 1 << 5;
    QTest::newRow("Incorrect slice") << volumeGeneric_1 << -1 << -1 << -2;
    QTest::newRow("Incorrect phase") << volumeGeneric_2 << 0 << -1 << -1;
}

void test_Volume::getImageIndex_ShouldReturnExpectedImageIndex()
{
    QFETCH(Volume*, volume);
    QFETCH(int, sliceNumber);
    QFETCH(int, phaseNumber);
    QFETCH(int, result);

    QCOMPARE(volume->getImageIndex(sliceNumber, phaseNumber), result);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::toString_ShouldReturnExpectedString_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<QString>("expectedString");

    QTest::newRow("data not loaded") << new Volume(this) << "Data are not loaded yet";

    int dimensions[3] = { 66, 66, 57 };
    int extent[6] = { 0, 65, 0, 65, 0, 56 };
    double spacing[3] = { 3.9, 3.9, 0.8 };
    double origin[3] = { -90.4, -48.7, -8.7 };
    VolumePixelData *pixelData = VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin);
    Volume *volume = new Volume(this);
    volume->setPixelData(pixelData);
    QString expectedString = "Dimensions: 66, 66, 57\nOrigin: -90.4, -48.7, -8.7\nSpacing: 3.9, 3.9, 0.8\nExtent: 0..65, 0..65, 0..56\nBounds: -90.4..163.1, -48.7..204.8, -8.7..36.1";
    QTest::newRow("data loaded") << volume << expectedString;
}

void test_Volume::toString_ShouldReturnExpectedString()
{
    QFETCH(Volume*, volume);
    QFETCH(QString, expectedString);

    QCOMPARE(volume->toString(), expectedString);
}

void test_Volume::getScalarPointer_ShouldReturnCorrectScalarPointer_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("z");
    QTest::addColumn<int>("value");

    QTest::newRow("up-left point") << 0 << 0 << 0 << 150;
    QTest::newRow("down-left point") << 0 << 9 << 0 << 150;
    QTest::newRow("up-right point") << 9 << 0 << 0 << 150;
    QTest::newRow("down-right point") << 9 << 9 << 0 << 150;
    QTest::newRow("middle-left point") << 0 << 5 << 0 << 70;
    QTest::newRow("middle-center point") << 5 << 5 << 0 << 70;
    QTest::newRow("middle-right point") << 9 << 5 << 0 << 70;

}

void test_Volume::getScalarPointer_ShouldReturnCorrectScalarPointer()
{
    QFETCH(int, x);
    QFETCH(int, y);
    QFETCH(int, z);
    QFETCH(int, value);

    Volume *volume = new Volume();
    volume->convertToNeutralVolume();

    void *voxelValue = volume->getScalarPointer(x,y,z);
    
    QCOMPARE(static_cast<short*>(voxelValue)[0], static_cast<short>(value));
    
    delete volume;
}

void test_Volume::isMultiframe_ShouldReturnTrueForMultiframeVolumes_data()
{
    QTest::addColumn< QSharedPointer<Volume> >("volume");
    QTest::addColumn<bool>("isMultiframe");

    QTest::newRow("singleframe 1 image") << QSharedPointer<Volume>(VolumeTestHelper::createVolume(1)) << false;
    QTest::newRow("singleframe n images") << QSharedPointer<Volume>(VolumeTestHelper::createVolume(5)) << false;
    QTest::newRow("multiframe") << QSharedPointer<Volume>(VolumeTestHelper::createMultiframeVolume(5)) << true;
}

void test_Volume::isMultiframe_ShouldReturnTrueForMultiframeVolumes()
{
    QFETCH(QSharedPointer<Volume>, volume);
    QFETCH(bool, isMultiframe);

    QCOMPARE(volume->isMultiframe(), isMultiframe);
}

DECLARE_TEST(test_Volume)

#include "test_volume.moc"
