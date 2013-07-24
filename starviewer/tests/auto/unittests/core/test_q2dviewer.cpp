#include "autotest.h"

#include "q2dviewer.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace udg;
using namespace testing;

class test_Q2DViewer : public QObject {
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

    void canShowDisplayShutter_ShouldReturnExpectedValue_data();
    void canShowDisplayShutter_ShouldReturnExpectedValue();

private:
    void setupGetIndexForViewShouldReturnMinusOneData();
};

Q_DECLARE_METATYPE(OrthogonalPlane::OrthogonalPlaneType)
Q_DECLARE_METATYPE(Q2DViewer*)
Q_DECLARE_METATYPE(Volume*)

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane::OrthogonalPlaneType>("view");
    QTest::addColumn<int>("xIndex");
    QTest::addColumn<int>("yIndex");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane::XYPlane << 0 << 1 << 2;
    QTest::newRow("YZ view") << OrthogonalPlane::YZPlane << 1 << 2 << 0;
    QTest::newRow("XZ view") << OrthogonalPlane::XZPlane << 0 << 2 << 1;
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane::OrthogonalPlaneType, view);
    QFETCH(int, xIndex);
    QFETCH(int, yIndex);
    QFETCH(int, zIndex);
    
    int x, y, z;
    Q2DViewer::getXYZIndexesForView(x, y, z, view);
    
    QCOMPARE(x, xIndex);
    QCOMPARE(y, yIndex);
    QCOMPARE(z, zIndex);
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    int x, y, z;
    Q2DViewer::getXYZIndexesForView(x, y, z, (OrthogonalPlane::OrthogonalPlaneType)view);
    
    QCOMPARE(x, -1);
    QCOMPARE(y, -1);
    QCOMPARE(z, -1);
}

void test_Q2DViewer::getXIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane::OrthogonalPlaneType>("view");
    QTest::addColumn<int>("xIndex");

    QTest::newRow("XY view") << OrthogonalPlane::XYPlane << 0;
    QTest::newRow("YZ view") << OrthogonalPlane::YZPlane << 1;
    QTest::newRow("XZ view") << OrthogonalPlane::XZPlane << 0;
}

void test_Q2DViewer::getXIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane::OrthogonalPlaneType, view);
    QFETCH(int, xIndex);
    
    QCOMPARE(Q2DViewer::getXIndexForView(view), xIndex);
}

void test_Q2DViewer::getXIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getXIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);    

    QCOMPARE(Q2DViewer::getXIndexForView((OrthogonalPlane::OrthogonalPlaneType)view), -1);
}

void test_Q2DViewer::getYIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane::OrthogonalPlaneType>("view");
    QTest::addColumn<int>("yIndex");

    QTest::newRow("XY view") << OrthogonalPlane::XYPlane << 1;
    QTest::newRow("YZ view") << OrthogonalPlane::YZPlane << 2;
    QTest::newRow("XZ view") << OrthogonalPlane::XZPlane << 2;
}

void test_Q2DViewer::getYIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane::OrthogonalPlaneType, view);
    QFETCH(int, yIndex);
    
    QCOMPARE(Q2DViewer::getYIndexForView(view), yIndex);
}

void test_Q2DViewer::getYIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getYIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(Q2DViewer::getYIndexForView((OrthogonalPlane::OrthogonalPlaneType)view), -1);
}

void test_Q2DViewer::getZIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<OrthogonalPlane::OrthogonalPlaneType>("view");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("XY view") << OrthogonalPlane::XYPlane << 2;
    QTest::newRow("YZ view") << OrthogonalPlane::YZPlane << 0;
    QTest::newRow("XZ view") << OrthogonalPlane::XZPlane << 1;
}

void test_Q2DViewer::getZIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(OrthogonalPlane::OrthogonalPlaneType, view);
    QFETCH(int, zIndex);
    
    QCOMPARE(Q2DViewer::getZIndexForView(view), zIndex);
}

void test_Q2DViewer::getZIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getZIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(Q2DViewer::getZIndexForView((OrthogonalPlane::OrthogonalPlaneType)view), -1);
}

void test_Q2DViewer::canShowDisplayShutter_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Q2DViewer*>("viewer");
    QTest::addColumn<bool>("expectedValue");
    QTest::addColumn<Volume*>("volumeToCleanup");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 1.0, 1.0, 1.0 };
    int dimensions[3] = { 8, 8, 2 };
    int extent[6] = { 0, 7, 0, 7, 0, 1 };
    DisplayShutter displayShutter;
    displayShutter.setShape(DisplayShutter::CircularShape);
    displayShutter.setPoints(QPoint(), 1);
    QList<DisplayShutter> displayShutterList;
    displayShutterList.append(displayShutter);

    Q2DViewer *viewer = new Q2DViewer();
    Volume *volume = 0;
    QTest::newRow("default viewer (no volume)") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    volume->setObjectName("Dummy Volume");
    viewer->setInput(volume);
    QTest::newRow("dummy volume") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    QTest::newRow("no thick slab & no XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    viewer->setSlabThickness(2);
    viewer->enableThickSlab();
    QTest::newRow("thick slab & no XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    QTest::newRow("no thick slab & XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    viewer->setSlabThickness(2);
    viewer->enableThickSlab();
    QTest::newRow("thick slab & XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("no thick slab & no XY plane & shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    viewer->setSlabThickness(2);
    viewer->enableThickSlab();
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("thick slab & no XY plane & shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("no thick slab & XY plane & shutter") << viewer << true << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, dimensions, extent);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    viewer->setSlabThickness(2);
    viewer->enableThickSlab();
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("thick slab & XY plane & shutter") << viewer << false << volume;
}

void test_Q2DViewer::canShowDisplayShutter_ShouldReturnExpectedValue()
{
    QFETCH(Q2DViewer*, viewer);
    QFETCH(bool, expectedValue);
    QFETCH(Volume*, volumeToCleanup);

    QCOMPARE(viewer->canShowDisplayShutter(), expectedValue);

    delete viewer;
    VolumeTestHelper::cleanUp(volumeToCleanup);
}

void test_Q2DViewer::setupGetIndexForViewShouldReturnMinusOneData()
{
    QTest::addColumn<int>("view");

    QTest::newRow("view is -2") << -2;
    QTest::newRow("view is -1") << -1;
    QTest::newRow("view is 3") << 3;
    QTest::newRow("view is 4") << 4;
}

DECLARE_TEST(test_Q2DViewer)

#include "test_q2dviewer.moc"
