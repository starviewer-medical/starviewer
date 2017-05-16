#include "autotest.h"
#include "slicehandler.h"

#include "volume.h"
#include "volumetesthelper.h"

using namespace udg;
using namespace testing;

class TestingSliceHandler : public SliceHandler {
public:
    TestingSliceHandler(QObject *parent = 0)
        : SliceHandler(parent), m_sliceLoopEnabled(false), m_phaseLoopEnabled(false)
    {
    }

    void setSliceRange(int min, int max)
    {
        m_minSliceValue = min;
        m_numberOfSlices = max + 1;
    }

    void setNumberOfPhases(int value)
    {
        m_numberOfPhases = value;
    }

public:
    bool m_sliceLoopEnabled;
    bool m_phaseLoopEnabled;

protected:
    bool isLoopEnabledForSlices() const
    {
        return m_sliceLoopEnabled;
    }

    bool isLoopEnabledForPhases() const
    {
        return m_phaseLoopEnabled;
    }
};

class test_SliceHandler : public QObject {
Q_OBJECT

private slots:
    void constructor_InitializesWithExpectedValues();

    void setVolume_UpdatesFieldsAsExpected();

    void setViewPlane_UpdatesViewPlaneSliceRangeAndMaximumSlabThickness_data();
    void setViewPlane_UpdatesViewPlaneSliceRangeAndMaximumSlabThickness();

    void setSlice_ComputesExpectedSlice_data();
    void setSlice_ComputesExpectedSlice();
    
    void setPhase_ComputesExpectedPhase_data();
    void setPhase_ComputesExpectedPhase();

    void setSlabThickness_UpdatesThickness_data();
    void setSlabThickness_UpdatesThickness();

    void setSlabThickness_DoesntUpdateThickness_data();
    void setSlabThickness_DoesntUpdateThickness();

    void setSlabThickness_ModifiesCurrentSliceAsExpected_data();
    void setSlabThickness_ModifiesCurrentSliceAsExpected();

    void getMaximumSlabThickness_ReturnsExpectedValue_data();
    void getMaximumSlabThickness_ReturnsExpectedValue();

    void getNumberOfSlicesInSlabThickness_ReturnsExpectedValue_data();
    void getNumberOfSlicesInSlabThickness_ReturnsExpectedValue();

};

Q_DECLARE_METATYPE(Volume*)
Q_DECLARE_METATYPE(OrthogonalPlane)

void test_SliceHandler::constructor_InitializesWithExpectedValues()
{
    SliceHandler sliceHandler;

    QCOMPARE(sliceHandler.getViewPlane(), OrthogonalPlane(OrthogonalPlane::XYPlane));
    QCOMPARE(sliceHandler.getCurrentSlice(), 0);
    QCOMPARE(sliceHandler.getMinimumSlice(), 0);
    QCOMPARE(sliceHandler.getMaximumSlice(), 0);
    QCOMPARE(sliceHandler.getNumberOfSlices(), 1);
    QCOMPARE(sliceHandler.getCurrentPhase(), 0);
    QCOMPARE(sliceHandler.getNumberOfPhases(), 1);
    QCOMPARE(sliceHandler.getSlabThickness(), 0.0);
    QCOMPARE(sliceHandler.getMaximumSlabThickness(), 0.0);
    QCOMPARE(sliceHandler.getNumberOfSlicesInSlabThickness(), 0);
    QCOMPARE(sliceHandler.getSliceThickness(), 0.0);
}

void test_SliceHandler::setVolume_UpdatesFieldsAsExpected()
{
    TestingSliceHandler sliceHandler;
    sliceHandler.setViewPlane(OrthogonalPlane::XZPlane);
    sliceHandler.setSliceRange(-10, 10);
    sliceHandler.setSlice(5);
    sliceHandler.setNumberOfPhases(30);
    sliceHandler.setPhase(10);
    sliceHandler.setSlabThickness(2.0);

    Volume *volume = VolumeTestHelper::createVolume(20, 4, 5);
    sliceHandler.setVolume(volume);

    QCOMPARE(sliceHandler.getViewPlane(), OrthogonalPlane(OrthogonalPlane::XYPlane));
    QCOMPARE(sliceHandler.getCurrentSlice(), 0);
    QCOMPARE(sliceHandler.getMinimumSlice(), 0);
    QCOMPARE(sliceHandler.getMaximumSlice(), 4);
    QCOMPARE(sliceHandler.getNumberOfSlices(), 5);
    QCOMPARE(sliceHandler.getCurrentPhase(), 0);
    QCOMPARE(sliceHandler.getNumberOfPhases(), 4);
    QCOMPARE(sliceHandler.getSlabThickness(), 0.0);
    QCOMPARE(sliceHandler.getMaximumSlabThickness(), 5.0);
    QCOMPARE(sliceHandler.getNumberOfSlicesInSlabThickness(), 0);
    QCOMPARE(sliceHandler.getSliceThickness(), 0.0);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::setViewPlane_UpdatesViewPlaneSliceRangeAndMaximumSlabThickness_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<OrthogonalPlane>("viewPlane");
    QTest::addColumn<int>("expectedMinimumSlice");
    QTest::addColumn<int>("expectedMaximumSlice");
    QTest::addColumn<double>("expectedMaximumSlabThickness");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 11, 5, 20, 1, 42 };

    QTest::newRow("XY") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << 1 << 42 << 84.0;
    QTest::newRow("YZ") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << 0 << 11 << 6.0;
    QTest::newRow("XZ") << VolumeTestHelper::createVolumeWithParameters(1, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << 5 << 20 << 8.0;
}

void test_SliceHandler::setViewPlane_UpdatesViewPlaneSliceRangeAndMaximumSlabThickness()
{
    QFETCH(Volume*, volume);
    QFETCH(OrthogonalPlane, viewPlane);
    QFETCH(int, expectedMinimumSlice);
    QFETCH(int, expectedMaximumSlice);
    QFETCH(double, expectedMaximumSlabThickness);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setViewPlane(viewPlane);

    QCOMPARE(sliceHandler.getMinimumSlice(), expectedMinimumSlice);
    QCOMPARE(sliceHandler.getMaximumSlice(), expectedMaximumSlice);
    QCOMPARE(sliceHandler.getMaximumSlabThickness(), expectedMaximumSlabThickness);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::setSlice_ComputesExpectedSlice_data()
{
    QTest::addColumn<bool>("loop");
    QTest::addColumn<int>("minSlice");
    QTest::addColumn<int>("maxSlice");
    QTest::addColumn<int>("sliceValue");
    QTest::addColumn<int>("expectedSliceValue");

    int minimumSlice = 0;
    int maximumSlice = 255;
    int sliceValueInsideRange = 10;
    QTest::newRow("No loop, value inside slice range") << false << minimumSlice << maximumSlice << sliceValueInsideRange << sliceValueInsideRange;
    QTest::newRow("Loop, value inside slice range") << true << minimumSlice << maximumSlice << sliceValueInsideRange << sliceValueInsideRange;
    
    int sliceValueBeyondMax = 600;
    QTest::newRow("No loop, value outside slice range (upper bound)") << false << minimumSlice << maximumSlice << sliceValueBeyondMax << maximumSlice;
    QTest::newRow("Loop, value outside slice range (upper bound)") << true << minimumSlice << maximumSlice << sliceValueBeyondMax << minimumSlice;

    int sliceValueBelowMin = -200;
    QTest::newRow("No loop, value outside slice range (lower bound)") << false << minimumSlice << maximumSlice << sliceValueBelowMin << minimumSlice;
    QTest::newRow("Loop, value outside slice range (lower bound)") << true << minimumSlice << maximumSlice << sliceValueBelowMin << maximumSlice;
}

void test_SliceHandler::setSlice_ComputesExpectedSlice()
{
    QFETCH(bool, loop);
    QFETCH(int, minSlice);
    QFETCH(int, maxSlice);
    QFETCH(int, sliceValue);
    QFETCH(int, expectedSliceValue);

    TestingSliceHandler sliceHandler;
    sliceHandler.m_sliceLoopEnabled = loop;
    sliceHandler.setSliceRange(minSlice, maxSlice);
    sliceHandler.setSlice(sliceValue);
    
    QCOMPARE(sliceHandler.getCurrentSlice(), expectedSliceValue);
}

void test_SliceHandler::setPhase_ComputesExpectedPhase_data()
{
    QTest::addColumn<bool>("loop");
    QTest::addColumn<int>("numberOfPhases");
    QTest::addColumn<int>("phaseValue");
    QTest::addColumn<int>("expectedPhaseValue");

    int numberOfPhases = 25;
    int phaseValueInsideRange = 10;
    QTest::newRow("No loop, value inside slice range") << false << numberOfPhases << phaseValueInsideRange << phaseValueInsideRange;
    QTest::newRow("Loop, value inside slice range") << true << numberOfPhases << phaseValueInsideRange << phaseValueInsideRange;
    
    int phaseValueBeyondMax = 600;
    QTest::newRow("No loop, value outside slice range (upper bound)") << false << numberOfPhases << phaseValueBeyondMax << numberOfPhases - 1;
    QTest::newRow("Loop, value outside slice range (upper bound)") << true << numberOfPhases << phaseValueBeyondMax << 0;

    int phaseValueBelowMin = -200;
    QTest::newRow("No loop, value outside slice range (lower bound)") << false << numberOfPhases << phaseValueBelowMin << 0;
    QTest::newRow("Loop, value outside slice range (lower bound)") << true << numberOfPhases << phaseValueBelowMin << numberOfPhases - 1;
}

void test_SliceHandler::setPhase_ComputesExpectedPhase()
{
    QFETCH(bool, loop);
    QFETCH(int, numberOfPhases);
    QFETCH(int, phaseValue);
    QFETCH(int, expectedPhaseValue);

    TestingSliceHandler sliceHandler;
    sliceHandler.m_phaseLoopEnabled = loop;
    sliceHandler.setNumberOfPhases(numberOfPhases);
    sliceHandler.setPhase(phaseValue);
    
    QCOMPARE(sliceHandler.getCurrentPhase(), expectedPhaseValue);
}

void test_SliceHandler::setSlabThickness_UpdatesThickness_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("initialSlabThickness");
    QTest::addColumn<double>("newSlabThickness");
    QTest::addColumn<double>("expectedNewSlabThickness");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 9, 0, 15, 0, 50 };

    QTest::newRow("Thickness 1") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent)
                                 << 5.0 << 1.0 << 1.0;
    QTest::newRow("Thickness inside range") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent)
                                            << 5.0 << 80.0 << 80.0;
    QTest::newRow("Thickness just inside range") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent)
                                                 << 5.0 << 102.0 << 102.0;
    QTest::newRow("Thickness just greater than range") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent)
                                                       << 5.0 << 103.0 << 102.0;
    QTest::newRow("Thickness greater than range") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent)
                                                  << 5.0 << 1000.0 << 102.0;
}

void test_SliceHandler::setSlabThickness_UpdatesThickness()
{
    QFETCH(Volume*, volume);
    QFETCH(double, initialSlabThickness);
    QFETCH(double, newSlabThickness);
    QFETCH(double, expectedNewSlabThickness);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setSlabThickness(initialSlabThickness);

    sliceHandler.setSlabThickness(newSlabThickness);

    QCOMPARE(sliceHandler.getSlabThickness(), expectedNewSlabThickness);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::setSlabThickness_DoesntUpdateThickness_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("initialSlabThickness");
    QTest::addColumn<double>("newSlabThickness");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 9, 0, 15, 0, 50 };

    QTest::newRow("Negative thickness") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent) << 5.0 << -1.0;
    QTest::newRow("Same thickness") << VolumeTestHelper::createVolumeWithParameters(51, 1, 1, origin, spacing, extent) << 5.0 << 5.0;
}

void test_SliceHandler::setSlabThickness_DoesntUpdateThickness()
{
    QFETCH(Volume*, volume);
    QFETCH(double, initialSlabThickness);
    QFETCH(double, newSlabThickness);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setSlabThickness(initialSlabThickness);

    sliceHandler.setSlabThickness(newSlabThickness);

    QCOMPARE(sliceHandler.getSlabThickness(), initialSlabThickness);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::setSlabThickness_ModifiesCurrentSliceAsExpected_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("initialSlabThickness");
    QTest::addColumn<int>("initialSlice");
    QTest::addColumn<double>("newSlabThickness");
    QTest::addColumn<int>("expectedCurrentSlice");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 9, 0, 15, 0, 255 };

    QTest::newRow("increase from 0 near the start") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                    << 0.0 << 3 << 200.0 << 50;
    QTest::newRow("increase from 0 near the middle") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                     << 0.0 << 114 << 200.0 << 114;
    QTest::newRow("increase from 0 near the end") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                  << 0.0 << 206 << 200.0 << 205;

    QTest::newRow("increase near the start") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                             << 77.0 << 19 << 112.0 << 28;
    QTest::newRow("increase near the middle") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                              << 77.0 << 100 << 112.0 << 100;
    QTest::newRow("increase near the end") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                           << 77.0 << 230 << 112.0 << 227;

    QTest::newRow("decrease near the start") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                             << 112.0 << 28 << 77.0 << 28;
    QTest::newRow("decrease near the middle") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                              << 112.0 << 100 << 77.0 << 100 ;
    QTest::newRow("decrease near the end") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                           << 112.0 << 227 << 77.0 << 227;

    QTest::newRow("decrease to 0 near the start") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                  << 200.0 << 50 << 0.0 << 50;
    QTest::newRow("decrease to 0 near the middle") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                   << 200.0 << 114 << 0.0 << 114 ;
    QTest::newRow("decrease to 0 near the end") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                                                << 200.0 << 205 << 0.0 << 205;
}

void test_SliceHandler::setSlabThickness_ModifiesCurrentSliceAsExpected()
{
    QFETCH(Volume*, volume);
    QFETCH(double, initialSlabThickness);
    QFETCH(int, initialSlice);
    QFETCH(double, newSlabThickness);
    QFETCH(int, expectedCurrentSlice);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setSlabThickness(initialSlabThickness);
    sliceHandler.setSlice(initialSlice);

    sliceHandler.setSlabThickness(newSlabThickness);

    QCOMPARE(sliceHandler.getCurrentSlice(), expectedCurrentSlice);
}

void test_SliceHandler::getMaximumSlabThickness_ReturnsExpectedValue_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<OrthogonalPlane>("viewPlane");
    QTest::addColumn<double>("expectedMaximumSlabThickness");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 9, 0, 15, 0, 255 };

    QTest::newRow("XY") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << 512.0;
    QTest::newRow("XZ") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << 8.0;
    QTest::newRow("YZ") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << 5.0;
}

void test_SliceHandler::getMaximumSlabThickness_ReturnsExpectedValue()
{
    QFETCH(Volume*, volume);
    QFETCH(OrthogonalPlane, viewPlane);
    QFETCH(double, expectedMaximumSlabThickness);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setViewPlane(viewPlane);

    QCOMPARE(sliceHandler.getMaximumSlabThickness(), expectedMaximumSlabThickness);

    VolumeTestHelper::cleanUp(volume);
}

void test_SliceHandler::getNumberOfSlicesInSlabThickness_ReturnsExpectedValue_data()
{
    QTest::addColumn<Volume*>("volume");
    QTest::addColumn<double>("slabThickness");
    QTest::addColumn<int>("expectedNumberOfSlices");

    double origin[3] = { 0.0, 0.0, 0.0 };
    double spacing[3] = { 0.5, 0.5, 2.0 };
    int extent[6] = { 0, 9, 0, 15, 0, 255 };

    QTest::newRow("no volume") << static_cast<Volume*>(nullptr) << 2.0 << 0;
    QTest::newRow("exact") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent) << 12.0 << 6;
    QTest::newRow("round up") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent) << 17.9 << 9;
    QTest::newRow("round down") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent) << 10.1 << 5;
    QTest::newRow("round 0.5") << VolumeTestHelper::createVolumeWithParameters(256, 1, 1, origin, spacing, extent) << 15.0 << 8;
}

void test_SliceHandler::getNumberOfSlicesInSlabThickness_ReturnsExpectedValue()
{
    QFETCH(Volume*, volume);
    QFETCH(double, slabThickness);
    QFETCH(int, expectedNumberOfSlices);

    SliceHandler sliceHandler;
    sliceHandler.setVolume(volume);
    sliceHandler.setSlabThickness(slabThickness);

    QCOMPARE(sliceHandler.getNumberOfSlicesInSlabThickness(), expectedNumberOfSlices);

    VolumeTestHelper::cleanUp(volume);
}

DECLARE_TEST(test_SliceHandler)

#include "test_slicehandler.moc"
