#include "autotest.h"
#include "orthogonalplane.h"

using namespace udg;

class test_OrthogonalPlane : public QObject {

    Q_OBJECT

private slots:
    void getXYZIndexes_ShouldReturnExpectedValues_data();
    void getXYZIndexes_ShouldReturnExpectedValues();

    void getXYZIndexes_ShouldReturnMinusOne_data();
    void getXYZIndexes_ShouldReturnMinusOne();

    void getXIndex_ShouldReturnExpectedValues_data();
    void getXIndex_ShouldReturnExpectedValues();

    void getXIndex_ShouldReturnMinusOne_data();
    void getXIndex_ShouldReturnMinusOne();

    void getYIndex_ShouldReturnExpectedValues_data();
    void getYIndex_ShouldReturnExpectedValues();

    void getYIndex_ShouldReturnMinusOne_data();
    void getYIndex_ShouldReturnMinusOne();

    void getZIndex_ShouldReturnExpectedValues_data();
    void getZIndex_ShouldReturnExpectedValues();

    void getZIndex_ShouldReturnMinusOne_data();
    void getZIndex_ShouldReturnMinusOne();

private:
    void setupGetIndexShouldReturnMinusOneData();
};

Q_DECLARE_METATYPE(OrthogonalPlane)

void test_OrthogonalPlane::getXYZIndexes_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("xIndex");
    QTest::addColumn<int>("yIndex");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0 << 1 << 2;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 1 << 2 << 0;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 0 << 2 << 1;
}

void test_OrthogonalPlane::getXYZIndexes_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, xIndex);
    QFETCH(int, yIndex);
    QFETCH(int, zIndex);

    int x, y, z;
    view.getXYZIndexes(x, y, z);

    QCOMPARE(x, xIndex);
    QCOMPARE(y, yIndex);
    QCOMPARE(z, zIndex);
}

void test_OrthogonalPlane::getXYZIndexes_ShouldReturnMinusOne_data()
{
    this->setupGetIndexShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getXYZIndexes_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    int x, y, z;
    OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getXYZIndexes(x, y, z);

    QCOMPARE(x, -1);
    QCOMPARE(y, -1);
    QCOMPARE(z, -1);
}

void test_OrthogonalPlane::getXIndex_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("xIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 1;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 0;
}

void test_OrthogonalPlane::getXIndex_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, xIndex);

    QCOMPARE(view.getXIndex(), xIndex);
}

void test_OrthogonalPlane::getXIndex_ShouldReturnMinusOne_data()
{
    this->setupGetIndexShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getXIndex_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getXIndex(), -1);
}

void test_OrthogonalPlane::getYIndex_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("yIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 1;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 2;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 2;
}

void test_OrthogonalPlane::getYIndex_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, yIndex);

    QCOMPARE(view.getYIndex(), yIndex);
}

void test_OrthogonalPlane::getYIndex_ShouldReturnMinusOne_data()
{
    this->setupGetIndexShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getYIndex_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getYIndex(), -1);
}

void test_OrthogonalPlane::getZIndex_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 2;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 0;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 1;
}

void test_OrthogonalPlane::getZIndex_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, zIndex);

    QCOMPARE(view.getZIndex(), zIndex);
}

void test_OrthogonalPlane::getZIndex_ShouldReturnMinusOne_data()
{
    this->setupGetIndexShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getZIndex_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getZIndex(), -1);
}

void test_OrthogonalPlane::setupGetIndexShouldReturnMinusOneData()
{
    QTest::addColumn<int>("view");

    QTest::newRow("view is -2") << -2;
    QTest::newRow("view is -1") << -1;
    QTest::newRow("view is 3") << 3;
    QTest::newRow("view is 4") << 4;
}


DECLARE_TEST(test_OrthogonalPlane)

#include "test_orthogonalplane.moc"

