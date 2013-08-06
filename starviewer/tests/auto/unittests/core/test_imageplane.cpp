#include "autotest.h"
#include "imageplane.h"

#include "image.h"
#include "mathtools.h"

using namespace udg;

class test_ImagePlane : public QObject {
Q_OBJECT

private slots:
    void getRowLength_ReturnsExpectedValues_data();
    void getRowLength_ReturnsExpectedValues();
    
    void getColumnLength_ReturnsExpectedValues_data();
    void getColumnLength_ReturnsExpectedValues();

    void fillFromImage_fillsExpectedValues_data();
    void fillFromImage_fillsExpectedValues();

    void getDistanceToPoint_ReturnsExpecteValues_data();
    void getDistanceToPoint_ReturnsExpecteValues();
};

Q_DECLARE_METATYPE(ImagePlane)
Q_DECLARE_METATYPE(double*)

void test_ImagePlane::getRowLength_ReturnsExpectedValues_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<double>("expectedLength");

    double randomXSpacing = MathTools::randomDouble(0.1, 5.6);
    double dummyYSpacing = 0.0;
    int randomColumns = MathTools::randomInt(128, 1024);
    
    ImagePlane plane;
    plane.setSpacing(PixelSpacing2D(randomXSpacing, dummyYSpacing));
    plane.setColumns(randomColumns);
    plane.setRows(0);

    QTest::newRow("Row length") << plane << randomXSpacing * randomColumns;
}

void test_ImagePlane::getRowLength_ReturnsExpectedValues()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(double, expectedLength);
    
    QCOMPARE(imagePlane.getRowLength(), expectedLength);
}

void test_ImagePlane::getColumnLength_ReturnsExpectedValues_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<double>("expectedLength");

    double dummyXSpacing = 0.0;
    double randomYSpacing = MathTools::randomDouble(0.1, 5.6);
    int  randomRows = MathTools::randomInt(128, 1024);
    
    ImagePlane plane;
    plane.setSpacing(PixelSpacing2D(dummyXSpacing, randomYSpacing));
    plane.setRows(randomRows);
    plane.setColumns(0);

    QTest::newRow("Column length") << plane << randomYSpacing * randomRows;
}

void test_ImagePlane::getColumnLength_ReturnsExpectedValues()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(double, expectedLength);
    
    QCOMPARE(imagePlane.getColumnLength(), expectedLength);
}

void test_ImagePlane::fillFromImage_fillsExpectedValues_data()
{
    QTest::addColumn<ImagePlane>("filledImagePlane");
    QTest::addColumn<ImagePlane>("expectedImagePlane");

    Image *image = new Image(this);
    ImagePlane filledPlane;
    filledPlane.fillFromImage(image);
    
    ImagePlane expectedPlane;
    expectedPlane.setImageOrientation(image->getImageOrientationPatient());
    expectedPlane.setSpacing(image->getPreferredPixelSpacing());
    expectedPlane.setThickness(image->getSliceThickness());
    expectedPlane.setRows(image->getRows());
    expectedPlane.setColumns(image->getColumns());
    expectedPlane.setOrigin(image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2]);
    
    QTest::newRow("Image plane is properly filled with empty image") << filledPlane << expectedPlane;

    // Some values to fill from
    ImageOrientation orientation;
    orientation.setDICOMFormattedImageOrientation("1\\2\\3\\4\\5\\6");
    PixelSpacing2D spacing(1.2, 2.1);
    double thickness = 3.0;
    int rows = 256;
    int columns = 128;
    double origin[3] = { 1.0, 2.0, 3.0 };
    
    image = new Image(this);
    image->setImageOrientationPatient(orientation);
    image->setPixelSpacing(spacing);
    image->setSliceThickness(thickness);
    image->setRows(rows);
    image->setColumns(columns);
    image->setImagePositionPatient(origin);

    expectedPlane.setImageOrientation(orientation);
    expectedPlane.setSpacing(spacing);
    expectedPlane.setThickness(thickness);
    expectedPlane.setRows(rows);
    expectedPlane.setColumns(columns);
    expectedPlane.setOrigin(origin);

    filledPlane.fillFromImage(image);

    QTest::newRow("Image plane is properly filled from image with random values") << filledPlane << expectedPlane;
}

void test_ImagePlane::fillFromImage_fillsExpectedValues()
{
    QFETCH(ImagePlane, filledImagePlane);
    QFETCH(ImagePlane, expectedImagePlane);

    QVERIFY(filledImagePlane == expectedImagePlane);
}

void test_ImagePlane::getDistanceToPoint_ReturnsExpecteValues_data()
{
    QTest::addColumn<double*>("point");
    QTest::addColumn<ImagePlane>("plane");
    QTest::addColumn<double>("expectedDistance");

    double *p1 = new double[3];
    p1[0] = 5.6;
    p1[1] = 8.7;
    p1[2] = -3.4;

    ImagePlane plane;
    plane.setOrigin(0.9, 1.4, 22.3);
    plane.setSpacing(PixelSpacing2D(1.25, 1.25));
    plane.setImageOrientation(ImageOrientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)));
    QTest::newRow("Distance to an axial plane") << p1 << plane << 25.7;
}

void test_ImagePlane::getDistanceToPoint_ReturnsExpecteValues()
{
    QFETCH(double*, point);
    QFETCH(ImagePlane, plane);
    QFETCH(double, expectedDistance);

    QCOMPARE(plane.getDistanceToPoint(point), expectedDistance);
}

DECLARE_TEST(test_ImagePlane)

#include "test_imageplane.moc"

