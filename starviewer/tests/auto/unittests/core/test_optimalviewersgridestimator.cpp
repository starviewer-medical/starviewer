#include "autotest.h"
#include "optimalviewersgridestimator.h"

using namespace udg;

class test_OptimalViewersGridEstimator : public QObject {
Q_OBJECT

private slots:
    void getOptimalGrid_ReturnsExpectedValues_data();
    void getOptimalGrid_ReturnsExpectedValues();
};

typedef QPair<int, int> GridPairType;
Q_DECLARE_METATYPE(GridPairType)

void test_OptimalViewersGridEstimator::getOptimalGrid_ReturnsExpectedValues_data()
{
    QTest::addColumn<int>("cells");
    QTest::addColumn<GridPairType>("expectedResult");

    GridPairType grid(1, 2);
    // 2x1 results
    for (int numberOfCells = 0; numberOfCells <= 2; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 2x2 results
    grid.first = 2;
    grid.second = 2;
    for (int numberOfCells = 3; numberOfCells <= 4; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 2x3 results
    grid.first = 3;
    grid.second = 2;
    for (int numberOfCells = 5; numberOfCells <= 6; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 4x2 results
    grid.first = 2;
    grid.second = 4;
    for (int numberOfCells = 7; numberOfCells <= 8; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 4x3 results
    grid.first = 3;
    grid.second = 4;
    for (int numberOfCells = 9; numberOfCells <= 12; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 4x4 results
    grid.first = 4;
    grid.second = 4;
    for (int numberOfCells = 13; numberOfCells <= 16; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 6x3 results
    grid.first = 3;
    grid.second = 6;
    for (int numberOfCells = 17; numberOfCells <= 18; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 4x5 results
    grid.first = 5;
    grid.second = 4;
    for (int numberOfCells = 19; numberOfCells <= 20; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 6x4 results
    grid.first = 4;
    grid.second = 6;
    for (int numberOfCells = 21; numberOfCells <= 24; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 4x7 results
    grid.first = 7;
    grid.second = 4;
    for (int numberOfCells = 25; numberOfCells <= 28; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 6x5 results
    grid.first = 5;
    grid.second = 6;
    for (int numberOfCells = 29; numberOfCells <= 30; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 8x4 results
    grid.first = 4;
    grid.second = 8;
    for (int numberOfCells = 31; numberOfCells <= 32; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 6x6 results
    grid.first = 6;
    grid.second = 6;
    for (int numberOfCells = 33; numberOfCells <= 36; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 8x5 results
    grid.first = 5;
    grid.second = 8;
    for (int numberOfCells = 37; numberOfCells <= 40; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 6x7 results
    grid.first = 7;
    grid.second = 6;
    for (int numberOfCells = 41; numberOfCells <= 42; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 8x6 results
    grid.first = 6;
    grid.second = 8;
    for (int numberOfCells = 43; numberOfCells <= 48; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }

    // 8x6 results for number of cells > maximum number of cells permitted (48)
    grid.first = 6;
    grid.second = 8;
    for (int numberOfCells = 49; numberOfCells <= 80; ++numberOfCells)
    {
        QTest::newRow(qPrintable(QString("%1 viewers").arg(numberOfCells))) << numberOfCells << grid;
    }
}

void test_OptimalViewersGridEstimator::getOptimalGrid_ReturnsExpectedValues()
{
    QFETCH(int, cells);
    QFETCH(GridPairType, expectedResult);
    
    OptimalViewersGridEstimator estimator;

    QCOMPARE(estimator.getOptimalGrid(cells), expectedResult);
}

DECLARE_TEST(test_OptimalViewersGridEstimator)

#include "test_optimalviewersgridestimator.moc"
