#include "autotest.h"
#include "imageplane.h"

#include "image.h"
#include "mathtools.h"

using namespace udg;

class test_ImagePlane : public QObject {
Q_OBJECT

private slots:
    void getCenter_ReturnsExpectedValue_data();
    void getCenter_ReturnsExpectedValue();

    void setCenter_ModifiesOriginCorrectly_data();
    void setCenter_ModifiesOriginCorrectly();

    void fillFromImage_fillsExpectedValues_data();
    void fillFromImage_fillsExpectedValues();

    void getCorners_ReturnsExpectedValues_data();
    void getCorners_ReturnsExpectedValues();

    void getIntersections_ComputesCorrectIntersections_data();
    void getIntersections_ComputesCorrectIntersections();

    void getDistanceToPoint_ReturnsExpectedValue_data();
    void getDistanceToPoint_ReturnsExpectedValue();

    void projectPoint_ReturnsExpectedValue_data();
    void projectPoint_ReturnsExpectedValue();

};

Q_DECLARE_METATYPE(ImagePlane)
Q_DECLARE_METATYPE(ImagePlane::CornersLocation)
Q_DECLARE_METATYPE(ImagePlane::Corners)
Q_DECLARE_METATYPE(Vector3)

void test_ImagePlane::getCenter_ReturnsExpectedValue_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<Vector3>("expectedCenter");

    ImagePlane plane;
    plane.setRowLength(10.0);
    plane.setColumnLength(5.0);

    QTest::newRow("XY (0,0,0) 10x5") << plane << Vector3(5.0, 2.5, 0.0);

    plane.setImageOrientation(ImageOrientation(QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    QTest::newRow("YZ (0,0,0) 10x5") << plane << Vector3(0.0, 5.0, 2.5);

    plane.setImageOrientation(ImageOrientation(QVector3D(0.0, 0.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    plane.setOrigin(0.0, 0.0, 100.0);
    plane.setColumnLength(20.0);

    QTest::newRow("Z-X (0,0,100) 10x20") << plane << Vector3(-10.0, 0.0, 105.0);
}

void test_ImagePlane::getCenter_ReturnsExpectedValue()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(Vector3, expectedCenter);

    QCOMPARE(imagePlane.getCenter(), expectedCenter);
}

void test_ImagePlane::setCenter_ModifiesOriginCorrectly_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<Vector3>("center");
    QTest::addColumn<Vector3>("expectedOrigin");

    ImagePlane plane;
    plane.setRowLength(10.0);
    plane.setColumnLength(5.0);

    QTest::newRow("XY (0,0,0) 10x5 <- c(5,5,5)") << plane << Vector3(5.0, 5.0, 5.0) << Vector3(0.0, 2.5, 5.0);

    plane.setImageOrientation(ImageOrientation(QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    QTest::newRow("YZ (0,0,0) 10x5 <- c(2,0,8)") << plane << Vector3(2.0, 0.0, 8.0) << Vector3(2.0, -5.0, 5.5);

    plane.setImageOrientation(ImageOrientation(QVector3D(0.0, 0.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    plane.setOrigin(0.0, 0.0, 100.0);
    plane.setColumnLength(20.0);

    QTest::newRow("Z-X (0,0,100) 10x20 <- c(0, 1, 2)") << plane << Vector3(0.0, 1.0, 2.0) << Vector3(10.0, 1.0, -3.0);
}

void test_ImagePlane::setCenter_ModifiesOriginCorrectly()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(Vector3, center);
    QFETCH(Vector3, expectedOrigin);

    imagePlane.setCenter(center);

    QCOMPARE(imagePlane.getOrigin(), expectedOrigin);
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
    expectedPlane.setRowLength(image->getColumns() * image->getPreferredPixelSpacing().x());
    expectedPlane.setColumnLength(image->getRows() * image->getPreferredPixelSpacing().y());
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
    expectedPlane.setRowLength(columns * spacing.x());
    expectedPlane.setColumnLength(rows * spacing.y());
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

void test_ImagePlane::getCorners_ReturnsExpectedValues_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<ImagePlane::CornersLocation>("cornersLocation");
    QTest::addColumn<ImagePlane::Corners>("expectedCorners");

    ImagePlane plane;
    plane.setImageOrientation(ImageOrientation(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    plane.setOrigin(10.0, 0.0, 2.0);
    plane.setRowLength(16.0);
    plane.setColumnLength(12.0);
    plane.setThickness(4.0);

    ImagePlane::Corners centralCorners = { ImagePlane::Central,
                                           Vector3(10.0, 0.0, 2.0), Vector3(26.0, 0.0, 2.0), Vector3(26.0, 0.0, 14.0), Vector3(10.0, 0.0, 14.0) };
    ImagePlane::Corners upperCorners = { ImagePlane::Upper,
                                         Vector3(10.0, -2.0, 2.0), Vector3(26.0, -2.0, 2.0), Vector3(26.0, -2.0, 14.0), Vector3(10.0, -2.0, 14.0) };
    ImagePlane::Corners lowerCorners = { ImagePlane::Lower,
                                         Vector3(10.0, 2.0, 2.0), Vector3(26.0, 2.0, 2.0), Vector3(26.0, 2.0, 14.0), Vector3(10.0, 2.0, 14.0) };

    QTest::newRow("central") << plane << ImagePlane::Central << centralCorners;
    QTest::newRow("upper") << plane << ImagePlane::Upper << upperCorners;
    QTest::newRow("lower") << plane << ImagePlane::Lower << lowerCorners;
}

void test_ImagePlane::getCorners_ReturnsExpectedValues()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(ImagePlane::CornersLocation, cornersLocation);
    QFETCH(ImagePlane::Corners, expectedCorners);

    auto corners = imagePlane.getCorners(cornersLocation);

    QCOMPARE(corners.location, expectedCorners.location);
    QCOMPARE(corners.topLeft, expectedCorners.topLeft);
    QCOMPARE(corners.topRight, expectedCorners.topRight);
    QCOMPARE(corners.bottomRight, expectedCorners.bottomRight);
    QCOMPARE(corners.bottomLeft, expectedCorners.bottomLeft);
}

void test_ImagePlane::getIntersections_ComputesCorrectIntersections_data()
{
    QTest::addColumn<ImagePlane>("imagePlane1");
    QTest::addColumn<ImagePlane>("imagePlane2");
    QTest::addColumn<ImagePlane::CornersLocation>("cornersLocation");
    QTest::addColumn<bool>("expectedReturnValue");
    QTest::addColumn<Vector3>("expectedIntersection1");
    QTest::addColumn<Vector3>("expectedIntersection2");

    ImagePlane plane1;
    plane1.setImageOrientation(ImageOrientation(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    plane1.setOrigin(10.0, 0.0, 2.0);
    plane1.setRowLength(16.0);
    plane1.setColumnLength(12.0);
    plane1.setThickness(4.0);

    QTest::newRow("same plane") << plane1 << plane1 << ImagePlane::Central << false << Vector3() << Vector3();

    ImagePlane plane2(plane1);
    plane2.setOrigin(0.0, 0.0, 0.0);

    QTest::newRow("parallel planes") << plane1 << plane2 << ImagePlane::Central << false << Vector3() << Vector3();

    ImagePlane plane3(plane1);
    plane3.setImageOrientation(ImageOrientation(QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    plane3.setCenter(plane1.getCenter());

    QTest::newRow("intersection at first try") << plane1 << plane3 << ImagePlane::Central << true << Vector3(18.0, 0.0, 2.0) << Vector3(18.0, 0.0, 14.0);

    ImagePlane plane4(plane1);
    plane4.setImageOrientation(ImageOrientation(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    plane4.setCenter(plane1.getCenter());

    QTest::newRow("intersection at second try") << plane1 << plane4 << ImagePlane::Central << true << Vector3(26.0, 0.0, 8.0) << Vector3(10.0, 0.0, 8.0);

    ImagePlane plane5(plane1);
    plane5.setImageOrientation(ImageOrientation(QVector3D(1.0, 1.0, -1.0).normalized(), QVector3D(-1.0, 0.0, -1.0).normalized()));
    plane5.setCenter(plane1.getCenter());

    QTest::newRow("central") << plane1 << plane5 << ImagePlane::Central << true << Vector3(12.0, 0.0, 2.0) << Vector3(24.0, 0.0, 14.0);
    QTest::newRow("upper") << plane1 << plane5 << ImagePlane::Upper << true << Vector3(8.0, -2.0, 2.0) << Vector3(20.0, -2.0, 14.0);
    QTest::newRow("lower") << plane1 << plane5 << ImagePlane::Lower << true << Vector3(16.0, 2.0, 2.0) << Vector3(28.0, 2.0, 14.0);
}

void test_ImagePlane::getIntersections_ComputesCorrectIntersections()
{
    QFETCH(ImagePlane, imagePlane1);
    QFETCH(ImagePlane, imagePlane2);
    QFETCH(ImagePlane::CornersLocation, cornersLocation);
    QFETCH(bool, expectedReturnValue);
    QFETCH(Vector3, expectedIntersection1);
    QFETCH(Vector3, expectedIntersection2);

    Vector3 intersection1, intersection2;

    QCOMPARE(imagePlane1.getIntersections(&imagePlane2, intersection1, intersection2, cornersLocation), expectedReturnValue);
    QCOMPARE(intersection1, expectedIntersection1);
    QCOMPARE(intersection2, expectedIntersection2);
}

void test_ImagePlane::getDistanceToPoint_ReturnsExpectedValue_data()
{
    QTest::addColumn<Vector3>("point");
    QTest::addColumn<ImagePlane>("plane");
    QTest::addColumn<double>("expectedDistance");

    Vector3 p1(5.6, 8.7, -3.4);

    ImagePlane plane;
    plane.setOrigin(0.9, 1.4, 22.3);
    plane.setSpacing(PixelSpacing2D(1.25, 1.25));
    plane.setImageOrientation(ImageOrientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)));
    QTest::newRow("Distance to an axial plane") << p1 << plane << 25.7;
}

void test_ImagePlane::getDistanceToPoint_ReturnsExpectedValue()
{
    QFETCH(Vector3, point);
    QFETCH(ImagePlane, plane);
    QFETCH(double, expectedDistance);

    QCOMPARE(plane.getDistanceToPoint(point), expectedDistance);
}

void test_ImagePlane::projectPoint_ReturnsExpectedValue_data()
{
    QTest::addColumn<ImagePlane>("imagePlane");
    QTest::addColumn<Vector3>("inputPoint");
    QTest::addColumn<Vector3>("expectedProjectedPoint");

    ImagePlane plane;
    plane.setImageOrientation(ImageOrientation(QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    plane.setOrigin(10.0, 0.0, 2.0);
    plane.setRowLength(16.0);
    plane.setColumnLength(12.0);
    plane.setThickness(4.0);

    QTest::newRow("point in plane bounds") << plane << Vector3(10.0, 0.0, 5.0) << Vector3(0.0, 3.0, 0.0);
    QTest::newRow("point in plane out of bounds") << plane << Vector3(0.0, 0.0, 0.0) << Vector3(-10.0, -2.0, 0.0);
    QTest::newRow("point out of plane") << plane << Vector3(-1.0, 5.0, 2.0) << Vector3(-11.0, 0.0, -5.0);
}

void test_ImagePlane::projectPoint_ReturnsExpectedValue()
{
    QFETCH(ImagePlane, imagePlane);
    QFETCH(Vector3, inputPoint);
    QFETCH(Vector3, expectedProjectedPoint);

    QCOMPARE(imagePlane.projectPoint(inputPoint), expectedProjectedPoint);
}

DECLARE_TEST(test_ImagePlane)

#include "test_imageplane.moc"
