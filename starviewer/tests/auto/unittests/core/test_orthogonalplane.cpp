#include "autotest.h"
#include "orthogonalplane.h"

using namespace udg;

class test_OrthogonalPlane : public QObject {

    Q_OBJECT

private slots:
    void getXYZIndexesForView_ShouldReturnExpectedValues_data();
    void getXYZIndexesForView_ShouldReturnExpectedValues();

    void getXYZIndexesForView_ShouldReturnMinusOne_data();
    void getXYZIndexesForView_ShouldReturnMinusOne();

    void getXIndexForView_ShouldReturnExpectedValues_data();
    void getXIndexForView_ShouldReturnExpectedValues();

    void getXIndexForView_ShouldReturnMinusOne_data();
    void getXIndexForView_ShouldReturnMinusOne();

    void getYIndexForView_ShouldReturnExpectedValues_data();
    void getYIndexForView_ShouldReturnExpectedValues();

    void getYIndexForView_ShouldReturnMinusOne_data();
    void getYIndexForView_ShouldReturnMinusOne();

    void getZIndexForView_ShouldReturnExpectedValues_data();
    void getZIndexForView_ShouldReturnExpectedValues();

    void getZIndexForView_ShouldReturnMinusOne_data();
    void getZIndexForView_ShouldReturnMinusOne();

private:
    void setupGetIndexForViewShouldReturnMinusOneData();
};

Q_DECLARE_METATYPE(OrthogonalPlane)

void test_OrthogonalPlane::getXYZIndexesForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("xIndex");
    QTest::addColumn<int>("yIndex");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0 << 1 << 2;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 1 << 2 << 0;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 0 << 2 << 1;
}

void test_OrthogonalPlane::getXYZIndexesForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, xIndex);
    QFETCH(int, yIndex);
    QFETCH(int, zIndex);

    int x, y, z;
    view.getXYZIndexesForView(x, y, z);

    QCOMPARE(x, xIndex);
    QCOMPARE(y, yIndex);
    QCOMPARE(z, zIndex);
}

void test_OrthogonalPlane::getXYZIndexesForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getXYZIndexesForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    int x, y, z;
    OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getXYZIndexesForView(x, y, z);

    QCOMPARE(x, -1);
    QCOMPARE(y, -1);
    QCOMPARE(z, -1);
}

void test_OrthogonalPlane::getXIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("xIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 1;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 0;
}

void test_OrthogonalPlane::getXIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, xIndex);

    QCOMPARE(view.getXIndexForView(), xIndex);
}

void test_OrthogonalPlane::getXIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getXIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getXIndexForView(), -1);
}

void test_OrthogonalPlane::getYIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("yIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 1;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 2;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 2;
}

void test_OrthogonalPlane::getYIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, yIndex);

    QCOMPARE(view.getYIndexForView(), yIndex);
}

void test_OrthogonalPlane::getYIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getYIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getYIndexForView(), -1);
}

void test_OrthogonalPlane::getZIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane>("view");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane(OrthogonalPlane::XYPlane) << 2;
    QTest::newRow("YZ view") << OrthogonalPlane(OrthogonalPlane::YZPlane) << 0;
    QTest::newRow("XZ view") << OrthogonalPlane(OrthogonalPlane::XZPlane) << 1;
}

void test_OrthogonalPlane::getZIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane, view);
    QFETCH(int, zIndex);

    QCOMPARE(view.getZIndexForView(), zIndex);
}

void test_OrthogonalPlane::getZIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_OrthogonalPlane::getZIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(OrthogonalPlane(static_cast<OrthogonalPlane::Plane>(view)).getZIndexForView(), -1);
}

void test_OrthogonalPlane::setupGetIndexForViewShouldReturnMinusOneData()
{
    QTest::addColumn<int>("view");

    QTest::newRow("view is -2") << -2;
    QTest::newRow("view is -1") << -1;
    QTest::newRow("view is 3") << 3;
    QTest::newRow("view is 4") << 4;
}


DECLARE_TEST(test_OrthogonalPlane)

#include "test_orthogonalplane.moc"

