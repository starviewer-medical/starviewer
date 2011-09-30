#include "autotest.h"

#include "displayshutter.h"

using namespace udg;

class test_DisplayShutter : public QObject {
Q_OBJECT
private slots:
    void setPoints_SetsCircularPoints_data();
    void setPoints_SetsCircularPoints();

    void setPoints_SetsRectangularPoints_data();
    void setPoints_SetsRectangularPoints();

    void setPoints_SetsPolygonalPoints_data();
    void setPoints_SetsPolygonalPoints();

    void intersection_ReturnsExpectedValues_data();
    void intersection_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DisplayShutter::ShapeType);
Q_DECLARE_METATYPE(QVector<QPoint>);
Q_DECLARE_METATYPE(QList<DisplayShutter>);
Q_DECLARE_METATYPE(DisplayShutter);

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

    shuttersList.clear();
    shuttersList << circularShutter;
    QTest::newRow("Single list (circular)") << shuttersList << circularShutter;
    
    DisplayShutter rectangularShutter;
    rectangularShutter.setPoints(QPoint(5, 233), QPoint(1018, 789));

    shuttersList.clear();
    shuttersList << rectangularShutter;
    QTest::newRow("Single list (rectangular)") << shuttersList << rectangularShutter;
    
    DisplayShutter polygonalShutter;
    QVector<QPoint> vertices;
    vertices << QPoint(1,1) << QPoint(1,5) << QPoint(5,5) << QPoint(10,6) << QPoint(8,6);
    polygonalShutter.setPoints(vertices);

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
}

DECLARE_TEST(test_DisplayShutter)

#include "test_displayshutter.moc"
