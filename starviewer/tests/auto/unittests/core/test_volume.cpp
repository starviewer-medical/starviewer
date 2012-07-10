#include "autotest.h"

#include "volume.h"
#include "image.h"
#include "fuzzycomparetesthelper.h"

#include <QVector3D>
#include <QSharedPointer>

using namespace udg;
using namespace testing;

class test_Volume : public QObject {
Q_OBJECT

private slots:
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
};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType)
Q_DECLARE_METATYPE(QList<Image*>)
Q_DECLARE_METATYPE(QSharedPointer<Volume>)
Q_DECLARE_METATYPE(ImageOrientation)

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
    QTest::addColumn<QList<Image*>>("volumeImages");
    QTest::addColumn<int>("slicesPerPhase");
    QTest::addColumn<int>("phase");
    QTest::addColumn<QList<Image*>>("phaseImages");

    Image *image1 = new Image();
    Image *image2 = new Image();
    Image *image3 = new Image();
    Image *image4 = new Image();
    Image *image5 = new Image();
    Image *image6 = new Image();

    QList<Image*> allImages, phase2Images;
    allImages << image1 << image2 << image3 << image4 << image5 << image6;

    phase2Images << image2 << image4 << image6;

    QTest::newRow("Volume with phases") << allImages << 3 << 1 << phase2Images;

    QTest::newRow("Volume without phases") << allImages << 6 << 0 << (QList<Image*>() << image1 << image2 << image3 << image4 << image5 << image6);

    //REPASSAR AQUEST
    QTest::newRow("Incorrect phase") << (QList<Image*>() << image1 << image2 << image3 << image4 << image5 << image6) << 2 << -1 << QList<Image*>();
}

void test_Volume::getPhaseImages_ShouldReturnExpectedPhaseImages()
{
    QFETCH(QList<Image*>, volumeImages);
    QFETCH(int, slicesPerPhase);
    QFETCH(int, phase);
    QFETCH(QList<Image*>, phaseImages);

    QSharedPointer<Volume> volume(new Volume());
    volume->setImages(volumeImages);
    volume->setNumberOfSlicesPerPhase(slicesPerPhase);
    volume->setNumberOfPhases(volumeImages.size()/slicesPerPhase);

    std::cout << "total:" << qPrintable(QString("%1").arg(volumeImages.size())) << std::endl;
    std::cout << "expected:" << qPrintable(QString("%1").arg(phaseImages.size())) << std::endl;
    std::cout << "real:" << qPrintable(QString("%1").arg(volume->getPhaseImages(phase).size())) << std::endl;

    QCOMPARE(volume->getPhaseImages(phase).size(), phaseImages.size());

    QCOMPARE(phaseImages, volume->getPhaseImages(phase));

    //ImageTestHelper::cleanUp(volume);
}

void test_Volume::getOrigin_ShouldReturnExpectedOrigin_data()
{
    QSharedPointer<Volume> volume(new Volume());
}

void test_Volume::getOrigin_ShouldReturnExpectedOrigin()
{

}

DECLARE_TEST(test_Volume)

#include "test_volume.moc"
