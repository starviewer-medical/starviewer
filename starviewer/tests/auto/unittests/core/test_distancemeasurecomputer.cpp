#include "autotest.h"

#include "distancemeasurecomputer.h"
#include "drawerline.h"
#include "pixelspacing2d.h"
#include "fuzzycomparetesthelper.h"
#include "mathtools.h"

using namespace udg;
using namespace testing;

class test_DistanceMeasureComputer : public QObject {
Q_OBJECT

private slots:
    void computeMeasureExplicit_ReturnsExpectedValues_data();
    void computeMeasureExplicit_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DrawerLine*)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(PixelSpacing2D)

void test_DistanceMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerLine*>("line");
    QTest::addColumn<double*>("dataSpacing");
    QTest::addColumn<PixelSpacing2D>("desiredSpacing");
    QTest::addColumn<double>("expectedMeasure");

    PixelSpacing2D dummySpacing2D(MathTools::randomDouble(0.1, 6.7), MathTools::randomDouble(0.1, 6.7));
    double *dummySpacing = new double[3];
    dummySpacing[0] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[1] = MathTools::randomDouble(0.1, 6.7);
    dummySpacing[2] = MathTools::randomDouble(0.1, 6.7);
    
    DrawerLine *line = 0;
    QTest::newRow("null line") << line << dummySpacing << dummySpacing2D << 0.0;
    
    double *spacing = 0;
    PixelSpacing2D desiredSpacing(1.2, 1.2);
    double p1[3] = { 8.99, 10.2, 8.97 };
    double p2[3] = { 2.34, 9.02, 8.97 };
    
    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 1.2, 1.2") << line << spacing << desiredSpacing << 6.75388;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    
    spacing = new double[3];
    spacing[0] = 0.75;
    spacing[1] = 0.75;
    spacing[2] = 3.5;
    QTest::newRow("Image spacing 1.2, 1.2 - Volume spacing 0.75, 0.75") << line << spacing << desiredSpacing << 10.8062;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    spacing = new double[3];
    spacing[0] = 1.0;
    spacing[1] = 1.0;
    spacing[2] = 1.0;
    QTest::newRow("Image spacing 0,0 - Volume spacing 1, 1") << line << spacing << PixelSpacing2D(0.0, 0.0) << 6.75388;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    
    spacing = new double[3];
    spacing[0] = 2.0;
    spacing[1] = 2.0;
    spacing[2] = 2.0;
    QTest::newRow("Image spacing 0,0 - Volume spacing 2, 2") << line << spacing << PixelSpacing2D(0.0, 0.0) << 3.37694;
}

void test_DistanceMeasureComputer::computeMeasureExplicit_ReturnsExpectedValues()
{
    QFETCH(DrawerLine*, line);
    QFETCH(double*, dataSpacing);
    QFETCH(PixelSpacing2D, desiredSpacing);
    QFETCH(double, expectedMeasure);
    
    DistanceMeasureComputer computer(line);
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computer.computeMeasureExplicit(dataSpacing, desiredSpacing), expectedMeasure, 0.00001));
}

DECLARE_TEST(test_DistanceMeasureComputer)

#include "test_distancemeasurecomputer.moc"