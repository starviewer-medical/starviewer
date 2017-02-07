#include "autotest.h"
#include "slicehandler.h"

#include "image.h"
#include "volume.h"
#include "volumetesthelper.h"

using namespace udg;
using namespace testing;

class TestingSliceHandler : public SliceHandler {
public:
    TestingSliceHandler() : m_sliceLoopEnabled(false), m_phaseLoopEnabled(false)
    {
    }
    void setPositionRange(double min, double max)
    {
        m_minPosition = min;
        m_maxPosition = max;
    }
    void setNumberOfPhases(int value)
    {
        m_numberOfPhases = value;
    }
public:
    bool m_sliceLoopEnabled;
    bool m_phaseLoopEnabled;
protected:
    bool isLoopEnabledForSlices() const override
    {
        return m_sliceLoopEnabled;
    }
    bool isLoopEnabledForPhases() const override
    {
        return m_phaseLoopEnabled;
    }
};

class test_SliceHandler : public QObject {

    Q_OBJECT

private slots:

    void constructor_InitializesWithExpectedValues();

    void setVolume_UpdatesFieldsAsExpected();

    void setReferenceViewPlane_UpdatesOrthogonalPlaneAndPositionBounds_data();
    void setReferenceViewPlane_UpdatesOrthogonalPlaneAndPositionBounds();

    void setOrthogonalViewPlane_UpdatesReferenceViewPlaneAndPositionBounds_data();
    void setOrthogonalViewPlane_UpdatesReferenceViewPlaneAndPositionBounds();

    void setOrthogonalViewPlane_ThrowsException();

    void setPosition_SetsExpectedPosition_data();
    void setPosition_SetsExpectedPosition();

    void getMinimumPosition_TakesSlabThicknessIntoAccount_data();
    void getMinimumPosition_TakesSlabThicknessIntoAccount();

    void getMaximumPosition_TakesSlabThicknessIntoAccount_data();
    void getMaximumPosition_TakesSlabThicknessIntoAccount();

    void getDefaultStepDistance_ReturnsExpectedValue_data();
    void getDefaultStepDistance_ReturnsExpectedValue();

    void getSlice_ReturnsExpectedValue_data();
    void getSlice_ReturnsExpectedValue();

    void setSlice_SetsExpectedSliceAndPosition_data();
    void setSlice_SetsExpectedSliceAndPosition();

    void getMinimumSlice_ReturnsExpectedValue_data();
    void getMinimumSlice_ReturnsExpectedValue();

    void getMaximumSlice_ReturnsExpectedValue_data();
    void getMaximumSlice_ReturnsExpectedValue();

    void getNumberOfSlices_ReturnsExpectedValue_data();
    void getNumberOfSlices_ReturnsExpectedValue();

    void setPhase_ComputesExpectedPhase_data();
    void setPhase_ComputesExpectedPhase();

    void setSlabThickness_UpdatesThicknessAndPosition_data();
    void setSlabThickness_UpdatesThicknessAndPosition();

    void getMaximumSlabThickness_ReturnsExpectedValue_data();
    void getMaximumSlabThickness_ReturnsExpectedValue();

    void getSliceThickness_ReturnsExpectedValue_data();
    void getSliceThickness_ReturnsExpectedValue();

    void getImage_ReturnsExpectedImage_data();
    void getImage_ReturnsExpectedImage();

};

Q_DECLARE_METATYPE(OrthogonalPlane)
Q_DECLARE_METATYPE(Plane*)
Q_DECLARE_METATYPE(Volume*)

void test_SliceHandler::constructor_InitializesWithExpectedValues()
{
    SliceHandler sliceHandler;

    QCOMPARE(sliceHandler.getReferenceViewPlane(), Plane(Vector3(0, 0, 1), Vector3()));
    QCOMPARE(sliceHandler.getOrthogonalViewPlane(), OrthogonalPlane(OrthogonalPlane::XYPlane));
    QCOMPARE(sliceHandler.getPosition(), 0.0);
    QCOMPARE(sliceHandler.getMinimumPosition(), 0.0);
    QCOMPARE(sliceHandler.getMaximumPosition(), 0.0);
    QCOMPARE(sliceHandler.getStepDistance(), 1.0);
    QCOMPARE(sliceHandler.getPhase(), 0);
    QCOMPARE(sliceHandler.getNumberOfPhases(), 1);
    QCOMPARE(sliceHandler.getSlabThickness(), 0.0);
    QCOMPARE(sliceHandler.getSnapToSlice(), true);
}

void test_SliceHandler::setVolume_UpdatesFieldsAsExpected()
{
    TestingSliceHandler sliceHandler;
    sliceHandler.setOrthogonalViewPlane(OrthogonalPlane::XZPlane);
    sliceHandler.setPositionRange(-10, 10);
    sliceHandler.setPosition(5);
    sliceHandler.setNumberOfPhases(30);
    sliceHandler.setPhase(10);
    sliceHandler.setSlabThickness(2.0);

    double origin[3] = { 0, -10, 12 };
    double spacing[3] = { 0.5, 0.5, 2 };
    int extent[6] = { 0, 255, 0, 255, 0, 19 };
    Volume *volume = VolumeTestHelper::createVolumeWithParameters(20, 4, 5, origin, spacing, extent);
    sliceHandler.setVolume(volume);

    QCOMPARE(sliceHandler.getReferenceViewPlane(), Plane(Vector3(0, 0, 1), Vector3(origin)));
    QCOMPARE(sliceHandler.getOrthogonalViewPlane(), OrthogonalPlane(OrthogonalPlane::XYPlane));
    QCOMPARE(sliceHandler.getPosition(), 0.0);
    QCOMPARE(sliceHandler.getMinimumPosition(), 0.0);
    QCOMPARE(sliceHandler.getMaximumPosition(), 8.0);
    QCOMPARE(sliceHandler.getStepDistance(), 2.0);
    QCOMPARE(sliceHandler.getPhase(), 0);
    QCOMPARE(sliceHandler.getNumberOfPhases(), 4);
    QCOMPARE(sliceHandler.getSlabThickness(), 0.0);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::setReferenceViewPlane_UpdatesOrthogonalPlaneAndPositionBounds_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<Plane*>("referenceViewPlane"); // has to be a pointer because it doesn't have a default constructor
    QTest::addColumn<OrthogonalPlane>("expectedOrthogonalViewPlane");
    QTest::addColumn<double>("expectedMinimumPosition");
    QTest::addColumn<double>("expectedMaximumPosition");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 11, 0, 20, 0, 42 };

    QTest::newRow("no phases +x 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(1, 0, 0), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::YZPlane) << 0.0 << 5.5;
    QTest::newRow("no phases -x 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(-1, 0, 0), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::None) << -5.5 << 0.0;
    QTest::newRow("no phases +y 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 1, 0), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::XZPlane) << 0.0 << 10.0;
    QTest::newRow("no phases -y 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, -1, 0), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::None) << -10.0 << 0.0;
    QTest::newRow("no phases +z 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, 1), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0.0 << 84.0;
    QTest::newRow("no phases -z 0") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, -1), Vector3())
                                    << OrthogonalPlane(OrthogonalPlane::None) << -84.0 << 0.0;

    QTest::newRow("no phases other plane") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                                           << new Plane(Vector3(1, 1, 1).normalize(), Vector3(0, 1, 100)) << OrthogonalPlane(OrthogonalPlane::None)
                                           << -58.3123771882 << -0.866025403784;

    extent[5] = 171;

    QTest::newRow("phases +z 0") << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, 1), Vector3())
                                 << OrthogonalPlane(OrthogonalPlane::XYPlane) << 0.0 << 84.0;
    QTest::newRow("phases -z 0") << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, -1), Vector3())
                                 << OrthogonalPlane(OrthogonalPlane::None) << -84.0 << 0.0;

    QTest::newRow("phases other plane")  << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent)
                                         << new Plane(Vector3(1, 1, 1).normalize(), Vector3(0, 1, 100)) << OrthogonalPlane(OrthogonalPlane::None)
                                         << -58.3123771882 << -0.866025403784;
}

void test_SliceHandler::setReferenceViewPlane_UpdatesOrthogonalPlaneAndPositionBounds()
{
    QFETCH(Volume*, volume);
    QFETCH(Plane*, referenceViewPlane);
    QFETCH(OrthogonalPlane, expectedOrthogonalViewPlane);
    QFETCH(double, expectedMinimumPosition);
    QFETCH(double, expectedMaximumPosition);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setReferenceViewPlane(*referenceViewPlane);

    QCOMPARE(sliceHandler.getOrthogonalViewPlane(), expectedOrthogonalViewPlane);
    QCOMPARE(sliceHandler.getMinimumPosition(), expectedMinimumPosition);
    QCOMPARE(sliceHandler.getMaximumPosition(), expectedMaximumPosition);

    VolumeTestHelper::cleanUp(volume);
    delete referenceViewPlane;
}

void test_SliceHandler::setOrthogonalViewPlane_UpdatesReferenceViewPlaneAndPositionBounds_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<OrthogonalPlane>("orthogonalViewPlane");
    QTest::addColumn<Plane*>("expectedReferenceViewPlane");
    QTest::addColumn<double>("expectedMinimumPosition");
    QTest::addColumn<double>("expectedMaximumPosition");
    QTest::addColumn<double>("expectedStepDistance");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 11, 0, 20, 0, 42 };

    QTest::newRow("no phases XY") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::XYPlane)
                                  << new Plane(Vector3(0, 0, 1), Vector3()) << 0.0 << 84.0 << 2.0;
    QTest::newRow("no phases YZ") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::YZPlane)
                                  << new Plane(Vector3(1, 0, 0), Vector3()) << 0.0 << 5.5 << 0.5;
    QTest::newRow("no phases XZ") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::XZPlane)
                                  << new Plane(Vector3(0, 1, 0), Vector3()) << 0.0 << 10.0 << 0.5;

    extent[5] = 171;

    QTest::newRow("phases XY") << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::XYPlane)
                               << new Plane(Vector3(0, 0, 1), Vector3()) << 0.0 << 84.0 << 2.0;
    QTest::newRow("phases YZ") << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::YZPlane)
                               << new Plane(Vector3(1, 0, 0), Vector3()) << 0.0 << 5.5 << 0.5;
    QTest::newRow("phases XZ") << VolumeTestHelper::createVolumeWithParameters(1, 4, 1, origin, spacing, extent) << OrthogonalPlane(OrthogonalPlane::XZPlane)
                               << new Plane(Vector3(0, 1, 0), Vector3()) << 0.0 << 10.0 << 0.5;
}

void test_SliceHandler::setOrthogonalViewPlane_UpdatesReferenceViewPlaneAndPositionBounds()
{
    QFETCH(Volume*, volume);
    QFETCH(OrthogonalPlane, orthogonalViewPlane);
    QFETCH(Plane*, expectedReferenceViewPlane);
    QFETCH(double, expectedMinimumPosition);
    QFETCH(double, expectedMaximumPosition);
    QFETCH(double, expectedStepDistance);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setOrthogonalViewPlane(orthogonalViewPlane);

    QCOMPARE(sliceHandler.getReferenceViewPlane(), *expectedReferenceViewPlane);
    QCOMPARE(sliceHandler.getMinimumPosition(), expectedMinimumPosition);
    QCOMPARE(sliceHandler.getMaximumPosition(), expectedMaximumPosition);
    QCOMPARE(sliceHandler.getStepDistance(), expectedStepDistance);

    VolumeTestHelper::cleanUp(volume);
    delete expectedReferenceViewPlane;
}

void test_SliceHandler::setOrthogonalViewPlane_ThrowsException()
{
    SliceHandler sliceHandler;

    QVERIFY_EXCEPTION_THROWN(sliceHandler.setOrthogonalViewPlane(OrthogonalPlane::None), std::invalid_argument);
}

void test_SliceHandler::setPosition_SetsExpectedPosition_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<bool>("snapToSlice");
    QTest::addColumn<double>("position");
    QTest::addColumn<double>("expectedPosition");

    QTest::newRow("normal no snap") << 0.0 << 10.0 << 1.0 << 0.0 << false << 5.1 << 5.1;
    QTest::newRow("normal snap back") << 0.0 << 10.0 << 1.0 << 0.0 << true << 5.1 << 5.0;
    QTest::newRow("normal snap forward") << 0.0 << 10.0 << 1.0 << 0.0 << true << 4.9 << 5.0;
    QTest::newRow("normal snap forward") << 0.0 << 10.0 << 1.0 << 0.0 << true << 4.5 << 5.0;
    QTest::newRow("lower than min no snap") << 0.0 << 10.0 << 1.0 << 0.0 << false << -1.0 << 0.0;
    QTest::newRow("greater than max no snap") << 0.0 << 10.0 << 1.0 << 0.0 << false << 11.0 << 10.0;
    QTest::newRow("lower than min snap") << -0.5 << 9.5 << 1.0 << 0.0 << true << -1.0 << 0.0;
    QTest::newRow("greater than max snap") << -0.5 << 9.5 << 1.0 << 0.0 << true << 11.0 << 10.0;
    QTest::newRow("lower than (min + half thickness) no snap") << 0.0 << 10.0 << 1.0 << 4.0 << false << 1.0 << 2.0;
    QTest::newRow("greater than (max - half thicknes) no snap") << 0.0 << 10.0 << 1.0 << 4.0 << false << 9.0 << 8.0;
    QTest::newRow("lower than (min + half thickness) snap") << -0.5 << 9.5 << 1.0 << 4.0 << true << 1.0 << 2.0;
    QTest::newRow("greater than (max - half thickness) snap") << -0.5 << 9.5 << 1.0 << 4.0 << true << 9.0 << 8.0;
}

void test_SliceHandler::setPosition_SetsExpectedPosition()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, stepDistance);
    QFETCH(double, slabThickness);
    QFETCH(bool, snapToSlice);
    QFETCH(double, position);
    QFETCH(double, expectedPosition);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(slabThickness);
    sliceHandler.setSnapToSlice(snapToSlice);
    sliceHandler.setPosition(position);

    QCOMPARE(sliceHandler.getPosition(), expectedPosition);
}

void test_SliceHandler::getMinimumPosition_TakesSlabThicknessIntoAccount_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<double>("expectedMinimumPosition");

    QTest::newRow("no thickness") << 0.0 << 10.0 << 0.0 << 0.0;
    QTest::newRow("thickness") << 0.0 << 10.0 << 2.0 << 1.0;
}

void test_SliceHandler::getMinimumPosition_TakesSlabThicknessIntoAccount()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, slabThickness);
    QFETCH(double, expectedMinimumPosition);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getMinimumPosition(), expectedMinimumPosition);
}

void test_SliceHandler::getMaximumPosition_TakesSlabThicknessIntoAccount_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<double>("expectedMaximumPosition");

    QTest::newRow("no thickness") << 0.0 << 10.0 << 0.0 << 10.0;
    QTest::newRow("thickness") << 0.0 << 10.0 << 2.0 << 9.0;
}

void test_SliceHandler::getMaximumPosition_TakesSlabThicknessIntoAccount()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, slabThickness);
    QFETCH(double, expectedMaximumPosition);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getMaximumPosition(), expectedMaximumPosition);
}

void test_SliceHandler::getDefaultStepDistance_ReturnsExpectedValue_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<Plane*>("referenceViewPlane");
    QTest::addColumn<double>("expectedDefaultStepDistance");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 1.0, 2.0 };
    int extent[6] = { 0, 11, 0, 20, 0, 42 };

    QTest::newRow("no volume") << static_cast<Volume*>(nullptr) << new Plane(Vector3(0, 0, 1), Vector3()) << 1.0;
    QTest::newRow("x spacing (+x)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(1, 0, 0), Vector3())
                                    << 0.5;
    QTest::newRow("x spacing (-x)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(-1, 0, 0), Vector3())
                                    << 0.5;
    QTest::newRow("y spacing (+y)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 1, 0), Vector3())
                                    << 1.0;
    QTest::newRow("y spacing (-y)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, -1, 0), Vector3())
                                    << 1.0;
    QTest::newRow("z spacing (+z)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, 1), Vector3())
                                    << 2.0;
    QTest::newRow("z spacing (-z)") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent) << new Plane(Vector3(0, 0, -1), Vector3())
                                    << 2.0;
    QTest::newRow("oblique plane") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                                   << new Plane(Vector3(0.5, -0.81, 0.42).normalize(), Vector3()) << 1.047482678984;
}

void test_SliceHandler::getDefaultStepDistance_ReturnsExpectedValue()
{
    QFETCH(Volume*, volume);
    QFETCH(Plane*, referenceViewPlane);
    QFETCH(double, expectedDefaultStepDistance);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setReferenceViewPlane(*referenceViewPlane);

    QCOMPARE(sliceHandler.getDefaultStepDistance(), expectedDefaultStepDistance);

    VolumeTestHelper::cleanUp(volume);
    delete referenceViewPlane;
}

void test_SliceHandler::getSlice_ReturnsExpectedValue_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("position");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<int>("expectedSlice");

    QTest::newRow("exact position") << 0.0 << 10.0 << 3.75 << 0.75 << 5;
    QTest::newRow("round down") << 0.0 << 10.0 << 4.0 << 0.75 << 5;
    QTest::newRow("round up") << 0.0 << 10.0 << 3.5 << 0.75 << 5;
    QTest::newRow("round up (.5)") << 0.0 << 10.0 << 3.375 << 0.75 << 5;
    QTest::newRow("different step distance") << 0.0 << 10.0 << 3.75 << 0.9 << 4;
}

void test_SliceHandler::getSlice_ReturnsExpectedValue()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, position);
    QFETCH(double, stepDistance);
    QFETCH(int, expectedSlice);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setSnapToSlice(false);
    sliceHandler.setPosition(position);
    sliceHandler.setStepDistance(stepDistance);

    QCOMPARE(sliceHandler.getSlice(), expectedSlice);
}

void test_SliceHandler::setSlice_SetsExpectedSliceAndPosition_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<bool>("loop");
    QTest::addColumn<int>("slice");
    QTest::addColumn<int>("expectedSlice");
    QTest::addColumn<double>("expectedPosition");

    QTest::newRow("inside range no loop") << 0.0 << 10.0 << 0.2 << 0.0 << false << 10 << 10 << 2.0;
    QTest::newRow("inside range loop") << 0.0 << 10.0 << 0.2 << 0.0 << true << 10 << 10 << 2.0;
    QTest::newRow("lower than min no loop") << 0.0 << 10.0 << 0.2 << 0.0 << false << -10 << 0 << 0.0;
    QTest::newRow("lower than min loop") << 0.0 << 10.0 << 0.2 << 0.0 << true << -10 << 50 << 10.0;
    QTest::newRow("greater than max no loop") << 0.0 << 10.0 << 0.2 << 0.0 << false << 55 << 50 << 10.0;
    QTest::newRow("greater than max loop") << 0.0 << 10.0 << 0.2 << 0.0 << true << 55 << 0 << 0.0;
    QTest::newRow("lower than (min + half thickness) no loop") << 0.0 << 10.0 << 0.2 << 4.0 << false << 3 << 10 << 2.0;
    QTest::newRow("lower than (min + half thickness) loop") << 0.0 << 10.0 << 0.2 << 4.0 << true << 5 << 40 << 8.0;
    QTest::newRow("greater than (max - half thicknes) no loop") << 0.0 << 10.0 << 0.2 << 4.0 << false << 45 << 40 << 8.0;
    QTest::newRow("greater than (max - half thickness) loop") << 0.0 << 10.0 << 0.2 << 4.0 << true << 45 << 10 << 2.0;
}

void test_SliceHandler::setSlice_SetsExpectedSliceAndPosition()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, stepDistance);
    QFETCH(double, slabThickness);
    QFETCH(bool, loop);
    QFETCH(int, slice);
    QFETCH(int, expectedSlice);
    QFETCH(double, expectedPosition);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(slabThickness);
    sliceHandler.m_sliceLoopEnabled = loop;
    sliceHandler.setSlice(slice);
    
    QCOMPARE(sliceHandler.getSlice(), expectedSlice);
    QCOMPARE(sliceHandler.getPosition(), expectedPosition);
}

void test_SliceHandler::getMinimumSlice_ReturnsExpectedValue_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<int>("expectedMinimumSlice");

    QTest::newRow("exact (no thickness)") << -10.0 << 10.0 << 1.0 << 0.0 << -10;
    QTest::newRow("round down (no thickness)") << -10.0 << 10.0 << 3.0 << 0.0 << -3;
    QTest::newRow("round up (no thickness)") << -10.0 << 10.0 << 1.5 << 0.0 << -7;
    QTest::newRow("exact (thickness)") << -10.0 << 10.0 << 1.0 << 2.0 << -9;
    QTest::newRow("round down (thickness)") << -10.0 << 10.0 << 3.0 << 1.5 << -3;
    QTest::newRow("round up (thickness)") << -10.0 << 10.0 << 1.5 << 2.5 << -6;
}

void test_SliceHandler::getMinimumSlice_ReturnsExpectedValue()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, stepDistance);
    QFETCH(double, slabThickness);
    QFETCH(int, expectedMinimumSlice);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getMinimumSlice(), expectedMinimumSlice);
}

void test_SliceHandler::getMaximumSlice_ReturnsExpectedValue_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<int>("expectedMaximumSlice");

    QTest::newRow("exact (no thickness)") << -10.0 << 10.0 << 1.0 << 0.0 << 10;
    QTest::newRow("round down (no thickness)") << -10.0 << 10.0 << 3.0 << 0.0 << 3;
    QTest::newRow("round up (no thickness)") << -10.0 << 10.0 << 1.5 << 0.0 << 7;
    QTest::newRow("exact (thickness)") << -10.0 << 10.0 << 1.0 << 2.0 << 9;
    QTest::newRow("round down (thickness)") << -10.0 << 10.0 << 3.0 << 1.5 << 3;
    QTest::newRow("round up (thickness)") << -10.0 << 10.0 << 1.5 << 2.5 << 6;
}

void test_SliceHandler::getMaximumSlice_ReturnsExpectedValue()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, stepDistance);
    QFETCH(double, slabThickness);
    QFETCH(int, expectedMaximumSlice);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getMaximumSlice(), expectedMaximumSlice);
}

void test_SliceHandler::getNumberOfSlices_ReturnsExpectedValue_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<int>("expectedNumberOfSlices");

    QTest::newRow("exact (no thickness)") << -10.0 << 10.0 << 1.0 << 0.0 << 21;
    QTest::newRow("round down (no thickness)") << -10.0 << 10.0 << 3.0 << 0.0 << 7;
    QTest::newRow("round up (no thickness)") << -10.0 << 10.0 << 1.5 << 0.0 << 15;
    QTest::newRow("exact (thickness ignored)") << -10.0 << 10.0 << 1.0 << 2.0 << 21;
    QTest::newRow("round down (thickness ignored)") << -10.0 << 10.0 << 3.0 << 1.5 << 7;
    QTest::newRow("round up (thickness ignored)") << -10.0 << 10.0 << 1.5 << 2.5 << 15;
}

void test_SliceHandler::getNumberOfSlices_ReturnsExpectedValue()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, stepDistance);
    QFETCH(double, slabThickness);
    QFETCH(int, expectedNumberOfSlices);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getNumberOfSlices(), expectedNumberOfSlices);
}

void test_SliceHandler::setPhase_ComputesExpectedPhase_data()
{
    QTest::addColumn<bool>("loop");
    QTest::addColumn<int>("numberOfPhases");
    QTest::addColumn<int>("phase");
    QTest::addColumn<int>("expectedPhase");

    int numberOfPhases = 25;
    int phaseInsideRange = 10;
    QTest::newRow("No loop, value inside slice range") << false << numberOfPhases << phaseInsideRange << phaseInsideRange;
    QTest::newRow("Loop, value inside slice range") << true << numberOfPhases << phaseInsideRange << phaseInsideRange;
    
    int phaseBeyondMax = 600;
    QTest::newRow("No loop, value outside slice range (upper bound)") << false << numberOfPhases << phaseBeyondMax << numberOfPhases - 1;
    QTest::newRow("Loop, value outside slice range (upper bound)") << true << numberOfPhases << phaseBeyondMax << 0;

    int phaseBelowMin = -200;
    QTest::newRow("No loop, value outside slice range (lower bound)") << false << numberOfPhases << phaseBelowMin << 0;
    QTest::newRow("Loop, value outside slice range (lower bound)") << true << numberOfPhases << phaseBelowMin << numberOfPhases - 1;
}

void test_SliceHandler::setPhase_ComputesExpectedPhase()
{
    QFETCH(bool, loop);
    QFETCH(int, numberOfPhases);
    QFETCH(int, phase);
    QFETCH(int, expectedPhase);

    TestingSliceHandler sliceHandler;
    sliceHandler.m_phaseLoopEnabled = loop;
    sliceHandler.setNumberOfPhases(numberOfPhases);
    sliceHandler.setPhase(phase);
    
    QCOMPARE(sliceHandler.getPhase(), expectedPhase);
}

void test_SliceHandler::setSlabThickness_UpdatesThicknessAndPosition_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("position");
    QTest::addColumn<double>("stepDistance");
    QTest::addColumn<double>("initialSlabThickness");
    QTest::addColumn<bool>("snapToSlice");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<double>("expectedPosition");
    QTest::addColumn<double>("expectedSlabThickness");

    QTest::newRow("negative thickness") << 0.0 << 100.0 << 51.0 << 5.0 << 10.0 << true << -1.0 << 51.0 << 10.0;
    QTest::newRow("same thickness") << 0.0 << 100.0 << 51.0 << 5.0 << 10.0 << true << 10.0 << 51.0 << 10.0;
    QTest::newRow("thickness greater than maximum") << 0.0 << 100.0 << 10.0 << 5.0 << 0.0 << true << 1000.0 << 50.0 << 100.0;
    QTest::newRow("snap to same slice") << 0.0 << 100.0 << 43.0 << 5.0 << 0.0 << true << 12.0 << 45.0 << 12.0;
    QTest::newRow("left-bounded snap") << 0.0 << 100.0 << 10.0 << 5.0 << 0.0 << true << 25.0 << 15.0 << 25.0;
    QTest::newRow("right-bounded snap") << 0.0 << 100.0 << 92.0 << 5.0 << 0.0 << true << 32.0 << 85.0 << 32.0;
    QTest::newRow("keep position") << 0.0 << 100.0 << 44.0 << 5.0 << 0.0 << false << 8.0 << 44.0 << 8.0;
    QTest::newRow("left-bounded no snap") << 0.0 << 100.0 << 3.0 << 5.0 << 0.0 << false << 25.0 << 12.5 << 25.0;
    QTest::newRow("right-bounded no snap") << 0.0 << 100.0 << 93.0 << 5.0 << 0.0 << false << 25.0 << 87.5 << 25.0;
}

void test_SliceHandler::setSlabThickness_UpdatesThicknessAndPosition()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, position);
    QFETCH(double, stepDistance);
    QFETCH(double, initialSlabThickness);
    QFETCH(bool, snapToSlice);
    QFETCH(double, slabThickness);
    QFETCH(double, expectedPosition);
    QFETCH(double, expectedSlabThickness);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);
    sliceHandler.setSnapToSlice(false);
    sliceHandler.setPosition(position);
    sliceHandler.setStepDistance(stepDistance);
    sliceHandler.setSlabThickness(initialSlabThickness);
    sliceHandler.setSnapToSlice(snapToSlice);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getPosition(), expectedPosition);
    QCOMPARE(sliceHandler.getSlabThickness(), expectedSlabThickness);
}

void test_SliceHandler::getMaximumSlabThickness_ReturnsExpectedValue_data()
{
    QTest::addColumn<double>("minPosition");
    QTest::addColumn<double>("maxPosition");
    QTest::addColumn<double>("expectedMaximumSlabThickness");

    QTest::newRow("1") << 0.0 << 100.0 << 100.0;
    QTest::newRow("2") << -35.8 << 0.0 << 35.8;
    QTest::newRow("3") << -99.3 << 29.3 << 128.6;
}

void test_SliceHandler::getMaximumSlabThickness_ReturnsExpectedValue()
{
    QFETCH(double, minPosition);
    QFETCH(double, maxPosition);
    QFETCH(double, expectedMaximumSlabThickness);

    TestingSliceHandler sliceHandler;
    sliceHandler.setPositionRange(minPosition, maxPosition);

    QCOMPARE(sliceHandler.getMaximumSlabThickness(), expectedMaximumSlabThickness);
}

void test_SliceHandler::getSliceThickness_ReturnsExpectedValue_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<Plane*>("referenceViewPlane");
    QTest::addColumn<double>("position");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<double>("expectedSliceThickness");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 1.0, 2.0 };
    int extent[6] = { 0, 11, 0, 20, 0, 42 };
    QTest::newRow("slab thickness") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                    << new Plane(Vector3(0, 0, 1), Vector3()) << 0.0 << 3.0 << 3.0;

    QTest::newRow("no volume") << static_cast<Volume*>(nullptr) << new Plane(Vector3(0, 0, 1), Vector3()) << 0.0 << 0.0 << 0.0;

    Volume *volume = VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent);
    volume->getImage(2)->setSliceThickness(8.0);
    QTest::newRow("exact image") << volume << new Plane(Vector3(0, 0, 1), Vector3()) << 4.0 << 0.0 << 8.0;

    QTest::newRow("x spacing (+x)") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                    << new Plane(Vector3(1, 0, 0), Vector3()) << 0.0 << 0.0 << 0.5;
    QTest::newRow("x spacing (-x)") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                    << new Plane(Vector3(-1, 0, 0), Vector3()) << 0.0 << 0.0 << 0.5;
    QTest::newRow("y spacing (+y)") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                    << new Plane(Vector3(0, 1, 0), Vector3()) << 0.0 << 0.0 << 1.0;
    QTest::newRow("y spacing (-y)") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                    << new Plane(Vector3(0, -1, 0), Vector3()) << 0.0 << 0.0 << 1.0;

    volume = VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent);
    volume->getImage(0)->setSliceThickness(8.0);
    QTest::newRow("first image thickness (+z)") << volume << new Plane(Vector3(0, 0, 1), Vector3()) << 1.2 << 0.0 << 8.0;

    volume = VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent);
    volume->getImage(0)->setSliceThickness(8.0);
    QTest::newRow("first image thickness (-z)") << volume << new Plane(Vector3(0, 0, -1), Vector3()) << 1.2 << 0.0 << 8.0;

    volume = VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent);
    volume->getImage(0)->setSliceThickness(8.0);
    QTest::newRow("oblique plane") << volume << new Plane(Vector3(0.5, -0.81, 0.42).normalize(), Vector3()) << 0.0 << 0.0 << 2.025219399538;
}

void test_SliceHandler::getSliceThickness_ReturnsExpectedValue()
{
    QFETCH(Volume*, volume);
    QFETCH(Plane*, referenceViewPlane);
    QFETCH(double, position);
    QFETCH(double, slabThickness);
    QFETCH(double, expectedSliceThickness);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setReferenceViewPlane(*referenceViewPlane);
    sliceHandler.setSnapToSlice(false);
    sliceHandler.setPosition(position);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getSliceThickness(), expectedSliceThickness);

    VolumeTestHelper::cleanUp(volume);
    delete referenceViewPlane;
}

void test_SliceHandler::getImage_ReturnsExpectedImage_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<Plane*>("referenceViewPlane");
    QTest::addColumn<double>("position");
    QTest::addColumn<Image*>("expectedImage");

    QTest::newRow("no volume") << static_cast<Volume*>(nullptr) << new Plane(Vector3(0, 0, 1), Vector3()) << 0.0 << static_cast<Image*>(nullptr);

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 1.0, 2.0 };
    int extent[6] = { 0, 11, 0, 20, 0, 42 };
    QTest::newRow("normal +x") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(1, 0, 0), Vector3())
                               << 0.0 << static_cast<Image*>(nullptr);
    QTest::newRow("normal -x") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(-1, 0, 0), Vector3())
                               << 0.0 << static_cast<Image*>(nullptr);
    QTest::newRow("normal +y") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(0, 1, 0), Vector3())
                               << 0.0 << static_cast<Image*>(nullptr);
    QTest::newRow("normal -y") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(0, -1, 0), Vector3())
                               << 0.0 << static_cast<Image*>(nullptr);
    QTest::newRow("normal -z") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(0, 0, -1), Vector3())
                               << 0.0 << static_cast<Image*>(nullptr);
    QTest::newRow("oblique plane") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent)
                                   << new Plane(Vector3(0.5, -0.81, 0.42).normalize(), Vector3()) << 0.0 << static_cast<Image*>(nullptr);

    Volume *volume = VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent);
    QTest::newRow("exact image position") << volume << new Plane(Vector3(0, 0, 1), Vector3()) << 8.0 << volume->getImage(4);

    QTest::newRow("betwen images") << VolumeTestHelper::createVolumeWithParameters(43, 1, 43, origin, spacing, extent) << new Plane(Vector3(0, 0, 1), Vector3())
                                   << 8.1 << static_cast<Image*>(nullptr);
}

void test_SliceHandler::getImage_ReturnsExpectedImage()
{
    QFETCH(Volume*, volume);
    QFETCH(Plane*, referenceViewPlane);
    QFETCH(double, position);
    QFETCH(Image*, expectedImage);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setReferenceViewPlane(*referenceViewPlane);
    sliceHandler.setSnapToSlice(false);
    sliceHandler.setPosition(position);

    QCOMPARE(sliceHandler.getImage(), expectedImage);

    VolumeTestHelper::cleanUp(volume);
    delete referenceViewPlane;
}

DECLARE_TEST(test_SliceHandler)

#include "test_slicehandler.moc"
