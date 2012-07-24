#include "autotest.h"

#include "volume.h"
#include "image.h"
#include "volumetesthelper.h"
#include "volumepixeldatatesthelper.h"
#include "imagetesthelper.h"
#include "itkandvtkimagetesthelper.h"
#include "fuzzycomparetesthelper.h"

#include <QVector3D>
#include <QSharedPointer>

#include <itkImageRegionConstIterator.h>

using namespace udg;
using namespace testing;

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

    void getAcquisitionPlane_ShouldReturnNotAvailable_data();
    void getAcquisitionPlane_ShouldReturnNotAvailable();

    void getAcquisitionPlane_ShouldReturnExpectedPlane_data();
    void getAcquisitionPlane_ShouldReturnExpectedPlane();

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

};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType)
Q_DECLARE_METATYPE(QList<Image*>)
Q_DECLARE_METATYPE(QSharedPointer<Volume>)
Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(ImageOrientation)
Q_DECLARE_METATYPE(Volume::ItkImageTypePointer)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkImageData>)
Q_DECLARE_METATYPE(VolumePixelData*)

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
        QCOMPARE(actualIterator.Get(), expectedIterator.Get());
        ++actualIterator;
        ++expectedIterator;
    }
}

void test_Volume::setData_vtk_ShouldBehaveAsExpected_data()
{
    QTest::addColumn<vtkSmartPointer<vtkImageData>>("vtkData");

    QTest::newRow("null") << vtkSmartPointer<vtkImageData>();
    QTest::newRow("not null") << vtkSmartPointer<vtkImageData>::New();
}

void test_Volume::setData_vtk_ShouldBehaveAsExpected()
{
    QFETCH(vtkSmartPointer<vtkImageData>, vtkData);

    Volume volume;
    volume.setData(vtkData);

    QCOMPARE(volume.isPixelDataLoaded(), true);

    vtkImageData *outputVtkData = volume.getVtkData();
    QCOMPARE(outputVtkData, vtkData.GetPointer());
}

void test_Volume::setPixelData_ShouldBehaveAsExpected_data()
{
    QTest::addColumn<VolumePixelData*>("pixelData");

    QTest::newRow("null") << static_cast<VolumePixelData*>(0);
    QTest::newRow("not null") << new VolumePixelData(this);
}

void test_Volume::setPixelData_ShouldBehaveAsExpected()
{
    QFETCH(VolumePixelData*, pixelData);

    Volume volume;
    volume.setPixelData(pixelData);

    QCOMPARE(volume.isPixelDataLoaded(), true);
    QCOMPARE(volume.getPixelData(), pixelData);
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
    
    QCOMPARE(volume.getAcquisitionPlane(), AnatomicalPlane::NotAvailable);
}

void test_Volume::getAcquisitionPlane_ShouldReturnExpectedPlane_data()
{
    QTest::addColumn<QList<Image*> >("imageSet");
    QTest::addColumn<AnatomicalPlane::AnatomicalPlaneType>("expectedResult");

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
    QTest::newRow("Axial (1 image)") << axialImages << AnatomicalPlane::Axial;

    QList<Image*> axialImages2;
    axialImages2 << axialImage;
    axialImages2 << axialImage;
    QTest::newRow("Axial (2 images)") << axialImages2 << AnatomicalPlane::Axial;

    QList<Image*> axialImages2MixedA;
    axialImages2MixedA << axialImage;
    axialImages2MixedA << sagittalImage;
    QTest::newRow("Axial (2 images, 2nd is sagittal)") << axialImages2MixedA << AnatomicalPlane::Axial;

    QList<Image*> axialImages2MixedB;
    axialImages2MixedB << axialImage;
    axialImages2MixedB << coronalImage;
    QTest::newRow("Axial (2 images, 2nd is coronal)") << axialImages2MixedB << AnatomicalPlane::Axial;

    QList<Image*> axialImages2MixedC;
    axialImages2MixedB << axialImage;
    axialImages2MixedB << obliqueImage;
    QTest::newRow("Axial (2 images, 2nd is oblique)") << axialImages2MixedB << AnatomicalPlane::Axial;
    
    // Casos que retornen Sagittal
    QList<Image*> sagittalImages;
    sagittalImages << sagittalImage;
    QTest::newRow("Sagittal (1 image)") << sagittalImages << AnatomicalPlane::Sagittal;
    
    QList<Image*> sagittalImages2;
    sagittalImages2 << sagittalImage;
    sagittalImages2 << sagittalImage;
    QTest::newRow("Sagittal (2 images)") << sagittalImages2 << AnatomicalPlane::Sagittal;

    QList<Image*> sagittalImages2MixedA;
    sagittalImages2MixedA << sagittalImage;
    sagittalImages2MixedA << axialImage;
    QTest::newRow("Sagittal (2 images, 2nd is axial)") << sagittalImages2MixedA << AnatomicalPlane::Sagittal;

    QList<Image*> sagittalImages2MixedB;
    sagittalImages2MixedB << sagittalImage;
    sagittalImages2MixedB << coronalImage;
    QTest::newRow("Sagittal (2 images, 2nd is coronal)") << sagittalImages2MixedB << AnatomicalPlane::Sagittal;

    QList<Image*> sagittalImages2MixedC;
    sagittalImages2MixedB << sagittalImage;
    sagittalImages2MixedB << obliqueImage;
    QTest::newRow("Sagittal (2 images, 2nd is oblique)") << sagittalImages2MixedB << AnatomicalPlane::Sagittal;
    
    // Casos que retornen Coronal
    QList<Image*> coronalImages;
    coronalImages << coronalImage;
    QTest::newRow("Coronal (1 image)") << coronalImages << AnatomicalPlane::Coronal;

    QList<Image*> coronalImages2;
    coronalImages2 << coronalImage;
    coronalImages2 << coronalImage;
    QTest::newRow("Coronal (2 images)") << coronalImages2 << AnatomicalPlane::Coronal;

    QList<Image*> coronalImages2MixedA;
    coronalImages2MixedA << coronalImage;
    coronalImages2MixedA << sagittalImage;
    QTest::newRow("Coronal (2 images, 2nd is sagittal)") << coronalImages2MixedA << AnatomicalPlane::Coronal;

    QList<Image*> coronalImages2MixedB;
    coronalImages2MixedB << coronalImage;
    coronalImages2MixedB << axialImage;
    QTest::newRow("Coronal (2 images, 2nd is axial)") << coronalImages2MixedB << AnatomicalPlane::Coronal;
    
    QList<Image*> coronalImages2MixedC;
    coronalImages2MixedB << coronalImage;
    coronalImages2MixedB << obliqueImage;
    QTest::newRow("Coronal (2 images, 2nd is oblique)") << coronalImages2MixedB << AnatomicalPlane::Coronal;

    // Casos que retornen Oblique
    QList<Image*> obliqueImages;
    obliqueImages << obliqueImage;
    QTest::newRow("Oblique (1 image)") << obliqueImages << AnatomicalPlane::Oblique;

    QList<Image*> obliqueImages2;
    obliqueImages2 << obliqueImage;
    obliqueImages2 << obliqueImage;
    QTest::newRow("Oblique (2 images)") << obliqueImages2 << AnatomicalPlane::Oblique;

    QList<Image*> obliqueImages2MixedA;
    obliqueImages2MixedA << obliqueImage;
    obliqueImages2MixedA << sagittalImage;
    QTest::newRow("Oblique (2 images, 2nd is sagittal)") << obliqueImages2MixedA << AnatomicalPlane::Oblique;

    QList<Image*> obliqueImages2MixedB;
    obliqueImages2MixedB << obliqueImage;
    obliqueImages2MixedB << axialImage;
    QTest::newRow("Oblique (2 images, 2nd is axial)") << obliqueImages2MixedB << AnatomicalPlane::Oblique;

    QList<Image*> obliqueImages2MixedC;
    obliqueImages2MixedC << obliqueImage;
    obliqueImages2MixedC << coronalImage;
    QTest::newRow("Oblique (2 images, 2nd is coronal)") << obliqueImages2MixedC << AnatomicalPlane::Oblique;
}

void test_Volume::getAcquisitionPlane_ShouldReturnExpectedPlane()
{
    QFETCH(QList<Image*>, imageSet);
    QFETCH(AnatomicalPlane::AnatomicalPlaneType, expectedResult);

    Volume volume;
    volume.setImages(imageSet);
    
    QCOMPARE(volume.getAcquisitionPlane(), expectedResult);
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
    QTest::addColumn<QVector3D>("normal");

    QSharedPointer<Volume> volume(new Volume());
    Image *image1 = new Image(volume.data());
    volume->addImage(image1);
    ImageOrientation imageOrientation;
    QVector3D positiveX(+1.0, 0.0, 0.0);
    QVector3D negativeX(-1.0, 0.0, 0.0);
    QVector3D positiveY(0.0, +1.0, 0.0);
    QVector3D negativeY(0.0, -1.0, 0.0);
    QVector3D positiveZ(0.0, 0.0, +1.0);
    QVector3D negativeZ(0.0, 0.0, -1.0);

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

    QVector3D row, column;

    row.setX(+3.9);
    row.setY(+9.5);
    row.setZ(+0.8);
    column.setX(-6.0);
    column.setY(-6.7);
    column.setZ(-3.2);
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (1)") << volume << imageOrientation << imageOrientation.getNormalVector();

    row.setX(+1.1);
    row.setY(+8.0);
    row.setZ(-4.7);
    column.setX(+3.5);
    column.setY(-8.4);
    column.setZ(+6.7);
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (2)") << volume << imageOrientation << imageOrientation.getNormalVector();

    row.setX(+3.0);
    row.setY(-0.6);
    row.setZ(+1.0);
    column.setX(-8.6);
    column.setY(+3.4);
    column.setZ(+7.2);
    imageOrientation.setRowAndColumnVectors(row, column);
    QTest::newRow("oblique (3)") << volume << imageOrientation << imageOrientation.getNormalVector();
}

void test_Volume::getStackDirection_ShouldReturnNormalVector()
{
    QFETCH(QSharedPointer<Volume>, volume);
    QFETCH(ImageOrientation, imageOrientation);
    QFETCH(QVector3D, normal);

    volume->getImage(0)->setImageOrientationPatient(imageOrientation);
    double dir[3];
    volume->getStackDirection(dir);
    QVector3D direction(dir[0], dir[1], dir[2]);

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
    QTest::addColumn<QList<Image*>>("phaseImages");

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

     int dimensions[3];
     double spacing[3];
     int extent[6];
     double origin[3];
     origin[0] = 10.56;
     origin[1] = -45.185;
     origin[2] = 12;
     Volume *volumeWithOrigin = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, dimensions, extent);

     double nullOrigin[3];
     nullOrigin[0] = NULL;
     nullOrigin[1] = NULL;
     nullOrigin[2] = NULL;
     Volume *volumeWithNullOrigin = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, nullOrigin, spacing, dimensions, extent);

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
    int extent[6];
    int dimensions[3];
    dimensions[0] = 128;
    dimensions[1] = -512;
    dimensions[2] = 45;
    Volume *volumeWithDimensions = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, dimensions, extent);

    int nullDimensions[3];
    nullDimensions[0] = NULL;
    nullDimensions[1] = NULL;
    nullDimensions[2] = NULL;
    Volume *volumeWithNullDimensions = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, nullDimensions, extent);

    QTest::newRow("Volume with dimensions") << volumeWithDimensions << dimensions[0] << dimensions[1] << dimensions[2];
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

void test_Volume::getSpacing_ShouldReturnExpectedSpacing_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("xValue");
    QTest::addColumn<double>("yValue");
    QTest::addColumn<double>("zValue");

    double origin[3];
    int dimensions[3];
    int extent[6];
    double spacing[3];
    spacing[0] = 3;
    spacing[1] = -2;
    spacing[2] = 2.5;
    Volume *volumeWithSpacing = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, dimensions, extent);

    double nullSpacing[3];
    nullSpacing[0] = NULL;
    nullSpacing[1] = NULL;
    nullSpacing[2] = NULL;
    Volume *volumeWithNullSpacing = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, nullSpacing, dimensions, extent);

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
    int dimensions[3];
    double spacing[3];
    int extent[6];
    extent[0] = 10;
    extent[1] = -2;
    extent[2] = 9;
    extent[3] = 2;
    extent[4] = 100;
    extent[5] = -500;

    Volume *volumeWithExtent = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, dimensions, extent);

    int nullExtent[6];
    nullExtent[0] = NULL;
    nullExtent[1] = NULL;
    nullExtent[2] = NULL;
    nullExtent[3] = NULL;
    nullExtent[4] = NULL;
    nullExtent[5] = NULL;
    
    Volume *volumeWithNullExtent = VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, dimensions, nullExtent);

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

    QCOMPARE(volume->getWholeExtent()[0], x1);
    QCOMPARE(volume->getWholeExtent()[1], x2);
    QCOMPARE(volume->getWholeExtent()[2], y1);
    QCOMPARE(volume->getWholeExtent()[3], y2);
    QCOMPARE(volume->getWholeExtent()[4], z1);
    QCOMPARE(volume->getWholeExtent()[5], z2);

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
    QTest::addColumn<QList<Image*>>("images");

    Volume *volumeWithTwoDifferentImages = VolumeTestHelper::createVolume();
    Image *image_1 = ImageTestHelper::createImageByUID("1");
    Image *image_2 = ImageTestHelper::createImageByUID("2");
    volumeWithTwoDifferentImages->addImage(image_1);
    volumeWithTwoDifferentImages->addImage(image_2);

    Volume *volumeWithDupplicate = VolumeTestHelper::createVolume();
    Image *image = ImageTestHelper::createImageByUID("1");
    volumeWithDupplicate->addImage(image);
    volumeWithDupplicate->addImage(image);

    QTest::newRow("Volume with two different images") << volumeWithTwoDifferentImages << (QList<Image*>() << image_1 << image_2);
    QTest::newRow("Volume with dipplicate") << volumeWithDupplicate << (QList<Image*>() << image);
}

void test_Volume::addImage_ShouldAddValidImages()
{
    QFETCH(Volume*, volume);
    QFETCH(QList<Image*>, images);

    QCOMPARE(volume->getImages().size(), images.size());
    QCOMPARE(volume->getImages(), images);

    VolumeTestHelper::cleanUp(volume);
}

void test_Volume::setImages_ShouldAddImageList_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<QList<Image*>>("images");
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

DECLARE_TEST(test_Volume)

#include "test_volume.moc"
