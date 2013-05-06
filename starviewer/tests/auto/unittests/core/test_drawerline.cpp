#include "autotest.h"
#include "drawerline.h"

#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_DrawerLine : public QObject {
Q_OBJECT

private slots:
    void computeDistance_ReturnsExpectedValue_data();
    void computeDistance_ReturnsExpectedValue();
};

Q_DECLARE_METATYPE(DrawerLine*)
Q_DECLARE_METATYPE(double*)

void test_DrawerLine::computeDistance_ReturnsExpectedValue_data()
{
    QTest::addColumn<DrawerLine*>("drawerLine");
    QTest::addColumn<double*>("spacing");
    QTest::addColumn<double>("expectedDistance");

    DrawerLine *line = 0;
    double p1[3] = { 8.99, 10.2, 8.97 };
    double p2[3] = { 2.34, 9.02, 8.97 };
    double *spacing = 0;
    
    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    spacing = 0;
    QTest::newRow("NULL spacing") << line << spacing << 6.75388;

    line = new DrawerLine(this);
    line->setFirstPoint(p1);
    line->setSecondPoint(p2);
    spacing = new double[3];
    spacing[0] = 1.2;
    spacing[1] = 1.2;
    spacing[2] = 3.5;
    QTest::newRow("With spacing - corrected distance") << line << spacing << 6.08276;
}

void test_DrawerLine::computeDistance_ReturnsExpectedValue()
{
    QFETCH(DrawerLine*, drawerLine);
    QFETCH(double*, spacing);
    QFETCH(double, expectedDistance);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(drawerLine->computeDistance(spacing), expectedDistance, 0.00001));
}

DECLARE_TEST(test_DrawerLine)

#include "test_drawerline.moc"
