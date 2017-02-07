#include "autotest.h"
#include "plane.h"

using namespace udg;

class test_Plane : public QObject {

    Q_OBJECT

private slots:

    void operatorEquals_ShouldReturnExpectedValue_data();
    void operatorEquals_ShouldReturnExpectedValue();

    void operatorNotEqual_ShouldReturnExpectedValue_data();
    void operatorNotEqual_ShouldReturnExpectedValue();

    void signedDistanceToPoint_ShouldReturnExpectedValue_data();
    void signedDistanceToPoint_ShouldReturnExpectedValue();

private:

    void setupComparisonData();

};

Q_DECLARE_METATYPE(Vector3)

void test_Plane::operatorEquals_ShouldReturnExpectedValue_data()
{
    setupComparisonData();
}

void test_Plane::operatorEquals_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, normal1);
    QFETCH(Vector3, point1);
    QFETCH(Vector3, normal2);
    QFETCH(Vector3, point2);
    QFETCH(bool, equals);

    QCOMPARE(Plane(normal1, point1) == Plane(normal2, point2), equals);
}

void test_Plane::operatorNotEqual_ShouldReturnExpectedValue_data()
{
    setupComparisonData();
}

void test_Plane::operatorNotEqual_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, normal1);
    QFETCH(Vector3, point1);
    QFETCH(Vector3, normal2);
    QFETCH(Vector3, point2);
    QFETCH(bool, equals);

    QCOMPARE(Plane(normal1, point1) != Plane(normal2, point2), !equals);
}

void test_Plane::signedDistanceToPoint_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3>("normal");
    QTest::addColumn<Vector3>("point");
    QTest::addColumn<Vector3>("distancePoint");
    QTest::addColumn<double>("expectedDistance");

    Vector3 x(1, 0, 0), nx(-1, 0, 0), y(0, 1, 0), zero;

    QTest::newRow("point in plane origin") << x << zero << zero << 0.0;
    QTest::newRow("point in plane") << x << zero << y << 0.0;
    QTest::newRow("point in normal direction") << x << zero << x << 1.0;
    QTest::newRow("point opposite normal direction") << x << zero << nx << -1.0;
}

void test_Plane::signedDistanceToPoint_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, normal);
    QFETCH(Vector3, point);
    QFETCH(Vector3, distancePoint);
    QFETCH(double, expectedDistance);

    QCOMPARE(Plane(normal, point).signedDistanceToPoint(distancePoint), expectedDistance);
}

void test_Plane::setupComparisonData()
{
    QTest::addColumn<Vector3>("normal1");
    QTest::addColumn<Vector3>("point1");
    QTest::addColumn<Vector3>("normal2");
    QTest::addColumn<Vector3>("point2");
    QTest::addColumn<bool>("equals");

    Vector3 x(1, 0, 0), nx(-1, 0, 0), zero;

    QTest::newRow("same plane") << x << zero << x << zero << true;
    QTest::newRow("inverse normal same point") << x << zero << nx << zero << false;
    QTest::newRow("same normal different point") << x << zero << x << x << false;
    QTest::newRow("different normal different point") << x << zero << nx << x << false;
}

DECLARE_TEST(test_Plane)

#include "test_plane.moc"
