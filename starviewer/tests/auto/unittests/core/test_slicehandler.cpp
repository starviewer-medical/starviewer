#include "autotest.h"
#include "slicehandler.h"

using namespace udg;

class TestingSliceHandler : public SliceHandler {
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
    void setSlice_ComputesExpectedSlice_data();
    void setSlice_ComputesExpectedSlice();
    
    void setPhase_ComputesExpectedPhase_data();
    void setPhase_ComputesExpectedPhase();

    void setSlabThickness_ComputesExpectedThickness_data();
    void setSlabThickness_ComputesExpectedThickness();

    void getLastSlabSlice_ReturnsExpectedValue_data();
    void getLastSlabSlice_ReturnsExpectedValue();

    void setSlabThickness_ModifiesCurrentSliceAsExpected_data();
    void setSlabThickness_ModifiesCurrentSliceAsExpected();
};

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

void test_SliceHandler::setSlabThickness_ComputesExpectedThickness_data()
{
    QTest::addColumn<int>("minSlice");
    QTest::addColumn<int>("maxSlice");
    QTest::addColumn<int>("slabThicknessValue");
    QTest::addColumn<int>("expectedSlabThicknessValue");

    int minimumSlice = 0;
    int maximumSlice = 255;
    QTest::newRow("Thickness inside range") << minimumSlice << maximumSlice << 200 << 200;
    QTest::newRow("Thickness greater than range") << minimumSlice << maximumSlice << 500 << 1;
}

void test_SliceHandler::setSlabThickness_ComputesExpectedThickness()
{
    QFETCH(int, minSlice);
    QFETCH(int, maxSlice);
    QFETCH(int, slabThicknessValue);
    QFETCH(int, expectedSlabThicknessValue);

    SliceHandler sliceHandler;
    sliceHandler.setSliceRange(minSlice, maxSlice);
    sliceHandler.setSlabThickness(slabThicknessValue);
    
    QCOMPARE(sliceHandler.getSlabThickness(), expectedSlabThicknessValue);
}

void test_SliceHandler::getLastSlabSlice_ReturnsExpectedValue_data()
{
    QTest::addColumn<int>("minSlice");
    QTest::addColumn<int>("maxSlice");
    QTest::addColumn<int>("slice");
    QTest::addColumn<int>("slabThicknessValue");
    QTest::addColumn<int>("expectedLastSlabSlice");

    int minimumSlice = 0;
    int maximumSlice = 255;
    int slice = 10;
    int slabThickness = 200;
    QTest::newRow("Slab inside range") << minimumSlice << maximumSlice << slice << slabThickness << 199;
}

void test_SliceHandler::getLastSlabSlice_ReturnsExpectedValue()
{
    QFETCH(int, minSlice);
    QFETCH(int, maxSlice);
    QFETCH(int, slice);
    QFETCH(int, slabThicknessValue);
    QFETCH(int, expectedLastSlabSlice);

    SliceHandler sliceHandler;
    sliceHandler.setSliceRange(minSlice, maxSlice);
    sliceHandler.setSlice(slice);
    sliceHandler.setSlabThickness(slabThicknessValue);
    
    QCOMPARE(sliceHandler.getLastSlabSlice(), expectedLastSlabSlice);
}

void test_SliceHandler::setSlabThickness_ModifiesCurrentSliceAsExpected_data()
{
    QTest::addColumn<int>("minSlice");
    QTest::addColumn<int>("maxSlice");
    QTest::addColumn<int>("slice");
    QTest::addColumn<int>("slabThicknessValue");
    QTest::addColumn<int>("expectedCurrentSlice");

    int minimumSlice = 0;
    int maximumSlice = 255;
    int slice = 50;
    int slabThickness = 25;
    QTest::newRow("Current slice decreases") << minimumSlice << maximumSlice << slice << slabThickness << 38;

    QTest::newRow("Current slice does not change") << minimumSlice << maximumSlice << slice << 2 << slice;
}

void test_SliceHandler::setSlabThickness_ModifiesCurrentSliceAsExpected()
{
    QFETCH(int, minSlice);
    QFETCH(int, maxSlice);
    QFETCH(int, slice);
    QFETCH(int, slabThicknessValue);
    QFETCH(int, expectedCurrentSlice);

    SliceHandler sliceHandler;
    sliceHandler.setSliceRange(minSlice, maxSlice);
    sliceHandler.setSlice(slice);
    sliceHandler.setSlabThickness(slabThicknessValue);

    QCOMPARE(sliceHandler.getCurrentSlice(), expectedCurrentSlice);
}

DECLARE_TEST(test_SliceHandler)

#include "test_slicehandler.moc"
