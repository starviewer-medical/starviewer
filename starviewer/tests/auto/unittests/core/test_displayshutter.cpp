#include "autotest.h"

#include "displayshutter.h"

#include <QColor>
#include <QPainter>

using namespace udg;

class test_DisplayShutter : public QObject {
Q_OBJECT
private slots:
    void getShapeAsDICOMString_ReturnsExpectedValues_data();
    void getShapeAsDICOMString_ReturnsExpectedValues();
    
    void getShutterValueAsQColor_ReturnsExpectedValues_data();
    void getShutterValueAsQColor_ReturnsExpectedValues();
    
    void setPoints_SetsCircularPoints_data();
    void setPoints_SetsCircularPoints();

    void setPoints_SetsRectangularPoints_data();
    void setPoints_SetsRectangularPoints();

    void setPoints_SetsPolygonalPoints_data();
    void setPoints_SetsPolygonalPoints();

    void setPoints_SetsPointsString_data();
    void setPoints_SetsPointsString();

    void getPointsAsString_ReturnsExpectedValues_data();
    void getPointsAsString_ReturnsExpectedValues();
    
    void intersection_ReturnsExpectedValues_data();
    void intersection_ReturnsExpectedValues();   

    void getAsQImage_ReturnsExpectedValues_data();
    void getAsQImage_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DisplayShutter::ShapeType);
Q_DECLARE_METATYPE(QVector<QPoint>);
Q_DECLARE_METATYPE(QList<DisplayShutter>);
Q_DECLARE_METATYPE(DisplayShutter);
Q_DECLARE_METATYPE(QColor);

void test_DisplayShutter::getShapeAsDICOMString_ReturnsExpectedValues_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shape");
    QTest::addColumn<QString>("dicomString");

    QTest::newRow("Undefined Shape") << DisplayShutter::UndefinedShape << "";
    QTest::newRow("Circular Shape") << DisplayShutter::CircularShape << "CIRCULAR";
    QTest::newRow("Rectangular Shape") << DisplayShutter::RectangularShape << "RECTANGULAR";
    QTest::newRow("Polygonal Shape") << DisplayShutter::PolygonalShape << "POLYGONAL";
}

void test_DisplayShutter::getShapeAsDICOMString_ReturnsExpectedValues()
{
    QFETCH(DisplayShutter::ShapeType, shape);
    QFETCH(QString, dicomString);

    DisplayShutter shutter;
    shutter.setShape(shape);
    
    QCOMPARE(shutter.getShapeAsDICOMString(), dicomString);
}

void test_DisplayShutter::getShutterValueAsQColor_ReturnsExpectedValues_data()
{
    QTest::addColumn<unsigned short int>("shutterValue");
    QTest::addColumn<QColor>("expectedQColor");

    unsigned short int value;
    
    value = 0x0;
    QTest::newRow("0x0->pure black") << value << QColor(Qt::black);
    value = 0xFFFF;
    QTest::newRow("0xFFFF->pure white") << value << QColor(Qt::white);
    value = 0x7FFF;
    QTest::newRow("0x7FFF->gray") << value << QColor::fromHsv(0, 0 , 127);
    value = 0x200;
    QTest::newRow("0x200->close to black") << value << QColor::fromHsv(0, 0 , 1);
    value = 0xFDFF;
    QTest::newRow("0xFF00->close to white") << value << QColor::fromHsv(0, 0 , 253);
    value = 0xBFBF;
    QTest::newRow("0xBFBF->lighter gray") << value << QColor::fromHsv(0, 0 , 191);
    value = 0x3F3F;
    QTest::newRow("0x3F3F->darker gray") << value << QColor::fromHsv(0, 0 , 63);
}

void test_DisplayShutter::getShutterValueAsQColor_ReturnsExpectedValues()
{
    QFETCH(unsigned short int, shutterValue);
    QFETCH(QColor, expectedQColor);

    DisplayShutter shutter;
    shutter.setShutterValue(shutterValue);
    
    QCOMPARE(shutter.getShutterValueAsQColor().rgba(), expectedQColor.rgba());
}

void test_DisplayShutter::setPoints_SetsCircularPoints_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shapeType");
    QTest::addColumn<QPoint>("centre");
    QTest::addColumn<int>("radius");
    QTest::addColumn<bool>("result");
    QTest::addColumn<DisplayShutter::ShapeType>("shapeAfterSetPoints");

    QTest::newRow("UndefinedShape turns to CircularShape, returns true") << DisplayShutter::UndefinedShape << QPoint() << 1 << true << DisplayShutter::CircularShape;
    QTest::newRow("CircularShape, keeps shape, returns true") << DisplayShutter::CircularShape << QPoint() << 1 << true << DisplayShutter::CircularShape;
    QTest::newRow("RectangularShape, keeps shape, no points set, returns false") << DisplayShutter::RectangularShape << QPoint() << 1 << false << DisplayShutter::RectangularShape;
    QTest::newRow("PolygonalShape, keeps shape, no points set, returns false") << DisplayShutter::PolygonalShape << QPoint() << 1 << false << DisplayShutter::PolygonalShape;
}

void test_DisplayShutter::setPoints_SetsCircularPoints()
{
    QFETCH(DisplayShutter::ShapeType, shapeType);
    QFETCH(QPoint, centre);
    QFETCH(int, radius);
    QFETCH(bool, result);
    QFETCH(DisplayShutter::ShapeType, shapeAfterSetPoints);

    DisplayShutter shutter;
    shutter.setShape(shapeType);
    QCOMPARE(shutter.setPoints(centre, radius), result);
    QVERIFY(shutter.getShape() == shapeAfterSetPoints);
}

void test_DisplayShutter::setPoints_SetsRectangularPoints_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shapeType");
    QTest::addColumn<QPoint>("topLeft");
    QTest::addColumn<QPoint>("bottomRight");
    QTest::addColumn<bool>("result");
    QTest::addColumn<DisplayShutter::ShapeType>("shapeAfterSetPoints");

    QPoint p1(5, 15);
    QPoint p2(10, 20);

    QTest::newRow("UndefinedShape turns to RectangularShape, returns true") << DisplayShutter::UndefinedShape << p1 << p2 << true << DisplayShutter::RectangularShape;
    QTest::newRow("RectangularShape, keeps shape, returns true") << DisplayShutter::RectangularShape << p1 << p2 << true << DisplayShutter::RectangularShape;
    QTest::newRow("CircularShape, keeps shape, no points set, returns false") << DisplayShutter::CircularShape << p1 << p2 << false << DisplayShutter::CircularShape;
    QTest::newRow("PolygonalShape, keeps shape, no points set, returns false") << DisplayShutter::PolygonalShape << p1 << p2 << false << DisplayShutter::PolygonalShape;
}

void test_DisplayShutter::setPoints_SetsRectangularPoints()
{
    QFETCH(DisplayShutter::ShapeType, shapeType);
    QFETCH(QPoint, topLeft);
    QFETCH(QPoint, bottomRight);
    QFETCH(bool, result);
    QFETCH(DisplayShutter::ShapeType, shapeAfterSetPoints);

    DisplayShutter shutter;
    shutter.setShape(shapeType);
    
    QCOMPARE(shutter.setPoints(topLeft, bottomRight), result);
    QVERIFY(shutter.getShape() == shapeAfterSetPoints);
}

void test_DisplayShutter::setPoints_SetsPolygonalPoints_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shapeType");
    QTest::addColumn<QVector<QPoint> >("points");
    QTest::addColumn<bool>("result");
    QTest::addColumn<DisplayShutter::ShapeType>("shapeAfterSetPoints");

    QVector<QPoint> polyPoints;
    polyPoints << QPoint(0, 0) << QPoint(3, 5) << QPoint(1, 53);
    
    QTest::newRow("UndefinedShape turns to PolygonalShape, returns true") << DisplayShutter::UndefinedShape << polyPoints << true << DisplayShutter::PolygonalShape;
    QTest::newRow("PolygonalShape, keeps shape, returns true") << DisplayShutter::PolygonalShape << polyPoints << true << DisplayShutter::PolygonalShape;
    QTest::newRow("CircularShape, keeps shape, no points set, returns false") << DisplayShutter::CircularShape << polyPoints << false << DisplayShutter::CircularShape;
    QTest::newRow("RectangularShape, keeps shape, no points set, returns false") << DisplayShutter::RectangularShape << polyPoints << false << DisplayShutter::RectangularShape;
}

void test_DisplayShutter::setPoints_SetsPolygonalPoints()
{
    QFETCH(DisplayShutter::ShapeType, shapeType);
    QFETCH(QVector<QPoint>, points);
    QFETCH(bool, result);
    QFETCH(DisplayShutter::ShapeType, shapeAfterSetPoints);

    DisplayShutter shutter;
    shutter.setShape(shapeType);
    QCOMPARE(shutter.setPoints(points), result);
    QVERIFY(shutter.getShape() == shapeAfterSetPoints);
}

void test_DisplayShutter::setPoints_SetsPointsString_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shape");
    QTest::addColumn<QString>("pointsString");
    QTest::addColumn<bool>("success");
    QTest::addColumn<DisplayShutter::ShapeType>("shapeAfterSetPoints");

    QString wellFormattedRectangleString("1,1;256,256");
    QString wellFormattedCircleString("256,256;50");
    QString wellFormattedPolygonString("1,1;1,3;3,3;3,8;9,10");
    QString noMatchingFormatString("abcde");
    
    QTest::newRow("Rectangle points, rectangle shape") << DisplayShutter::RectangularShape << wellFormattedRectangleString << true << DisplayShutter::RectangularShape;
    QTest::newRow("Circle points, circle shape") << DisplayShutter::CircularShape << wellFormattedCircleString << true << DisplayShutter::CircularShape;
    QTest::newRow("Polygon points, polygonal shape") << DisplayShutter::PolygonalShape << wellFormattedPolygonString << true << DisplayShutter::PolygonalShape;
    QTest::newRow("Rectangle points, undefined shape") << DisplayShutter::UndefinedShape << wellFormattedRectangleString << true << DisplayShutter::RectangularShape;
    QTest::newRow("Circle points, undefined shape") << DisplayShutter::UndefinedShape << wellFormattedCircleString << true << DisplayShutter::CircularShape;
    QTest::newRow("Polygon points, undefined shape") << DisplayShutter::UndefinedShape << wellFormattedPolygonString << true << DisplayShutter::PolygonalShape;
    QTest::newRow("Rectangle points, bad match (circle shape)") << DisplayShutter::CircularShape << wellFormattedRectangleString << false << DisplayShutter::CircularShape;
    QTest::newRow("Rectangle points, bad match (polygon shape)") << DisplayShutter::PolygonalShape << wellFormattedRectangleString << false << DisplayShutter::PolygonalShape;
    QTest::newRow("Circle points, bad match (rectangle shape)") << DisplayShutter::RectangularShape << wellFormattedCircleString << false << DisplayShutter::RectangularShape;
    QTest::newRow("Circle points, bad match (polygon shape)") << DisplayShutter::PolygonalShape << wellFormattedCircleString << false << DisplayShutter::PolygonalShape;
    QTest::newRow("Polygon points, bad match (rectangle shape)") << DisplayShutter::RectangularShape << wellFormattedPolygonString << false << DisplayShutter::RectangularShape;
    QTest::newRow("Polygon points, bad match (circle shape)") << DisplayShutter::CircularShape << wellFormattedPolygonString << false << DisplayShutter::CircularShape;
    QTest::newRow("Bad string points, rectangle shape") << DisplayShutter::RectangularShape << noMatchingFormatString << false << DisplayShutter::RectangularShape;
    QTest::newRow("Bad string points, circle shape") << DisplayShutter::CircularShape << noMatchingFormatString << false << DisplayShutter::CircularShape;
    QTest::newRow("Bad string points, polygon shape") << DisplayShutter::PolygonalShape << noMatchingFormatString << false << DisplayShutter::PolygonalShape;
    QTest::newRow("Bad string points, undefined shape") << DisplayShutter::UndefinedShape << noMatchingFormatString << false << DisplayShutter::UndefinedShape;
}

void test_DisplayShutter::setPoints_SetsPointsString()
{
    QFETCH(DisplayShutter::ShapeType, shape);
    QFETCH(QString, pointsString);
    QFETCH(bool, success);
    QFETCH(DisplayShutter::ShapeType, shapeAfterSetPoints);

    DisplayShutter shutter;
    shutter.setShape(shape);
    QCOMPARE(shutter.setPoints(pointsString), success);
    QVERIFY(shutter.getShape() == shapeAfterSetPoints);
}

void test_DisplayShutter::getPointsAsString_ReturnsExpectedValues_data()
{
    QTest::addColumn<DisplayShutter::ShapeType>("shape");
    QTest::addColumn<QString>("pointsString");
    QTest::addColumn<QString>("expectedString");

    QString wellFormattedRectangleString("1,1;256,256");
    QString wellFormattedCircleString("256,256;50");
    QString wellFormattedPolygonString("1,1;1,3;3,3;3,8;9,10");

    QTest::newRow("Shutter with rectangular shape") << DisplayShutter::RectangularShape << wellFormattedRectangleString << wellFormattedRectangleString;
    QTest::newRow("Shutter with circular shape") << DisplayShutter::CircularShape << wellFormattedCircleString << wellFormattedCircleString;
    QTest::newRow("Shutter with polygonal shape") << DisplayShutter::PolygonalShape << wellFormattedPolygonString << wellFormattedPolygonString;
    QTest::newRow("Shutter with undefined shape") << DisplayShutter::UndefinedShape << QString() << QString();
}

void test_DisplayShutter::getPointsAsString_ReturnsExpectedValues()
{
    QFETCH(DisplayShutter::ShapeType, shape);
    QFETCH(QString, pointsString);
    QFETCH(QString, expectedString);
    
    DisplayShutter shutter;

    shutter.setShape(shape);
    shutter.setPoints(pointsString);

    QCOMPARE(shutter.getPointsAsString(), expectedString);
}

void test_DisplayShutter::intersection_ReturnsExpectedValues_data()
{
    QTest::addColumn<QList<DisplayShutter> >("shuttersList");
    QTest::addColumn<DisplayShutter>("intersectedShutter");

    QList<DisplayShutter> shuttersList;
    QTest::newRow("Empty list") << shuttersList << DisplayShutter();

    DisplayShutter emptyShutter;
    
    shuttersList.clear();
    shuttersList << emptyShutter;
    QTest::newRow("Single list ('empty' shutter)") << shuttersList << emptyShutter;
    
    DisplayShutter circularShutter;
    circularShutter.setPoints(QPoint(512, 512), 517);
    circularShutter.setShutterValue(0xFF00);

    shuttersList.clear();
    shuttersList << circularShutter;
    QTest::newRow("Single list (circular)") << shuttersList << circularShutter;
    
    DisplayShutter rectangularShutter;
    rectangularShutter.setPoints(QPoint(5, 233), QPoint(1018, 789));
    rectangularShutter.setShutterValue(0x3F3F);

    shuttersList.clear();
    shuttersList << rectangularShutter;
    QTest::newRow("Single list (rectangular)") << shuttersList << rectangularShutter;
    
    DisplayShutter polygonalShutter;
    QVector<QPoint> vertices;
    vertices << QPoint(1,1) << QPoint(1,5) << QPoint(5,5) << QPoint(10,6) << QPoint(8,6);
    polygonalShutter.setPoints(vertices);
    polygonalShutter.setShutterValue(0xFFFF);

    shuttersList.clear();
    shuttersList << polygonalShutter;
    QTest::newRow("Single list (polygonal)") << shuttersList << polygonalShutter;

    shuttersList.clear();
    shuttersList << circularShutter << rectangularShutter;
    
    QPolygon shutterPoints;
    shutterPoints << QPoint(1018,608) << QPoint(1012,640) << QPoint(992,702) << QPoint(965,761) << QPoint(947,789) << QPoint(76,789) << QPoint(58,761)
        << QPoint(31,702) << QPoint(11,640) << QPoint(5,605) << QPoint(5,418) << QPoint(11,383) << QPoint(31,321) << QPoint(58,262) << QPoint(77,233)
        << QPoint(946,233) << QPoint(965,262) << QPoint(992,321) << QPoint(1012,383) << QPoint(1018,415) << QPoint(1018,608);
    
    DisplayShutter intersectedShutter;
    intersectedShutter.setPoints(shutterPoints);
    intersectedShutter.setShutterValue(0x9F1F);
    QTest::newRow("2-item list with intersection (circular+rectangular)") << shuttersList << intersectedShutter;

    shuttersList.clear();
    shuttersList << circularShutter << polygonalShutter;
    QTest::newRow("2-item list, no intersection (circular+polygonal)") << shuttersList << emptyShutter;

    shuttersList.clear();
    shuttersList << circularShutter << rectangularShutter << polygonalShutter;
    QTest::newRow("3-item list (circular+rectangular+polygonal), circular and rectangular intersect, but polygonal does not") << shuttersList << emptyShutter;
}

void test_DisplayShutter::intersection_ReturnsExpectedValues()
{
    QFETCH(QList<DisplayShutter>, shuttersList);
    QFETCH(DisplayShutter, intersectedShutter);

    DisplayShutter resultingShutter = DisplayShutter::intersection(shuttersList);
    
    QCOMPARE(resultingShutter.getShape(), intersectedShutter.getShape());
    QCOMPARE(resultingShutter.getAsQPolygon(), intersectedShutter.getAsQPolygon());
    QCOMPARE(resultingShutter.getShutterValue(), intersectedShutter.getShutterValue());
}

void test_DisplayShutter::getAsQImage_ReturnsExpectedValues_data()
{
    QTest::addColumn<DisplayShutter>("shutter");
    QTest::addColumn<QImage>("expectedQImage");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");

    int width = 10;
    int height = 10;
    
    QImage undefinedShutterImage(width, height, QImage::Format_RGB32);
    undefinedShutterImage.fill(Qt::black);
    undefinedShutterImage.invertPixels();
    QTest::newRow("Undefined shutter") << DisplayShutter() << undefinedShutterImage << width << height;

    DisplayShutter rectangularShutter;
    rectangularShutter.setPoints(QPoint(1, 1), QPoint(5, 5));
    
    QImage rectangularShutterImage(width, height, QImage::Format_RGB32);
    rectangularShutterImage.fill(Qt::black);
    
    QPainter rectangularPainter(&rectangularShutterImage);
    rectangularPainter.setPen(Qt::white);
    rectangularPainter.setBrush(Qt::white);
    rectangularPainter.drawRect(1, 1, 4, 4);
    rectangularShutterImage.invertPixels();

    QTest::newRow("rectangular shutter") << rectangularShutter << rectangularShutterImage << width << height;

    DisplayShutter circularShutter;
    circularShutter.setPoints(QPoint(5, 5), 3);

    QImage circularShutterImage(width, height, QImage::Format_RGB32);
    circularShutterImage.fill(Qt::black);
    
    QPainter circularPainter(&circularShutterImage);
    circularPainter.setPen(Qt::white);
    circularPainter.setBrush(Qt::white);
    circularPainter.drawPolygon(circularShutter.getAsQPolygon());
    circularShutterImage.invertPixels();

    QTest::newRow("circular shutter") << circularShutter << circularShutterImage << width << height;

    QPolygon polygon;
    polygon << QPoint(1,1) << QPoint(6,2) << QPoint(3,8);

    DisplayShutter polygonalShutter;
    polygonalShutter.setPoints(polygon);

    QImage polygonalShutterImage(width, height, QImage::Format_RGB32);
    polygonalShutterImage.fill(Qt::black);
    
    QPainter polygonalPainter(&polygonalShutterImage);
    polygonalPainter.setPen(Qt::white);
    polygonalPainter.setBrush(Qt::white);
    polygonalPainter.drawPolygon(polygon);
    polygonalShutterImage.invertPixels();

    QTest::newRow("polygonal shutter") << polygonalShutter << polygonalShutterImage << width << height;
}

void test_DisplayShutter::getAsQImage_ReturnsExpectedValues()
{
    QFETCH(DisplayShutter, shutter);
    QFETCH(QImage, expectedQImage);
    QFETCH(int, width);
    QFETCH(int, height);
    
    QCOMPARE(shutter.getAsQImage(width, height), expectedQImage);
}

DECLARE_TEST(test_DisplayShutter)

#include "test_displayshutter.moc"
