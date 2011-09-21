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
};

Q_DECLARE_METATYPE(DisplayShutter::ShapeType);
Q_DECLARE_METATYPE(QVector<QPoint>);

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

DECLARE_TEST(test_DisplayShutter)

#include "test_displayshutter.moc"
