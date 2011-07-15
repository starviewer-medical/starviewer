#include "autotest.h"

#include "volume.h"
#include "image.h"

using namespace udg;

class test_Volume : public QObject {
Q_OBJECT

private slots:
    void getAcquisitionPlane_ShouldReturnNotAvailable_data();
    void getAcquisitionPlane_ShouldReturnNotAvailable();

    void getAcquisitionPlane_ShouldReturnExpectedPlane_data();
    void getAcquisitionPlane_ShouldReturnExpectedPlane();
};

Q_DECLARE_METATYPE(AnatomicalPlane::AnatomicalPlaneType)
Q_DECLARE_METATYPE(QList<Image*>)

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

DECLARE_TEST(test_Volume)

#include "test_volume.moc"
