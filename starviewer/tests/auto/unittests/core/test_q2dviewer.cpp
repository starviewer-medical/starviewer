#include "autotest.h"

#include "q2dviewer.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

#include <QProcessEnvironment>

using namespace udg;
using namespace testing;

class test_Q2DViewer : public QObject {
Q_OBJECT

private slots:
    void canShowDisplayShutter_ShouldReturnExpectedValue_data();
    void canShowDisplayShutter_ShouldReturnExpectedValue();
};

Q_DECLARE_METATYPE(Q2DViewer*)
Q_DECLARE_METATYPE(Volume*)


void test_Q2DViewer::canShowDisplayShutter_ShouldReturnExpectedValue_data()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

    if (environment.contains("APPVEYOR") || environment.value("TRAVIS_OS_NAME") == "linux")
    {
        QSKIP("Test crashes in AppVeyor and Travis CI Linux");
    }

    QTest::addColumn<Q2DViewer*>("viewer");
    QTest::addColumn<bool>("expectedValue");
    QTest::addColumn<Volume*>("volumeToCleanup");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 1.0, 1.0, 1.0 };
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
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    volume->setObjectName("Dummy Volume");
    viewer->setInput(volume);
    QTest::newRow("dummy volume") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    QTest::newRow("no thick slab & no XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    viewer->setSlabThickness(2);
    QTest::newRow("thick slab & no XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    QTest::newRow("no thick slab & XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    viewer->setSlabThickness(2);
    QTest::newRow("thick slab & XY plane & no shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("no thick slab & no XY plane & shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::YZPlane);
    viewer->setSlabThickness(2);
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("thick slab & no XY plane & shutter") << viewer << false << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    volume->getImage(0)->setDisplayShutters(displayShutterList);
    QTest::newRow("no thick slab & XY plane & shutter") << viewer << true << volume;

    viewer = new Q2DViewer();
    volume = VolumeTestHelper::createVolumeWithParameters(2, 1, 1, origin, spacing, extent, true);
    viewer->setInput(volume);
    viewer->enableRendering(false);
    viewer->resetView(OrthogonalPlane::XYPlane);
    viewer->setSlabThickness(2);
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

DECLARE_TEST(test_Q2DViewer)

#include "test_q2dviewer.moc"
