#include "autotest.h"
#include "griditerator.h"

using namespace udg;

class test_GridIterator : public QObject {
    Q_OBJECT

private slots:
    void constructor_ShouldPerformExpectedInitialization_data();
    void constructor_ShouldPerformExpectedInitialization();

    void next_ShouldUpdateIndicesAsExpected_data();
    void next_ShouldUpdateIndicesAsExpected();

    void isAtEnd_ShouldReturnExpectedValue_data();
    void isAtEnd_ShouldReturnExpectedValue();

};

void test_GridIterator::constructor_ShouldPerformExpectedInitialization_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");

    QTest::newRow("1x1") << 1 << 1;
    QTest::newRow("1x2") << 1 << 2;
    QTest::newRow("2x1") << 2 << 1;
    QTest::newRow("3x3") << 3 << 3;
}

void test_GridIterator::constructor_ShouldPerformExpectedInitialization()
{
    QFETCH(int, rows);
    QFETCH(int, columns);

    GridIterator it(rows, columns);

    QCOMPARE(it.rows(), rows);
    QCOMPARE(it.columns(), columns);
    QCOMPARE(it.row(), 0);
    QCOMPARE(it.column(), 0);
    QCOMPARE(it.isAtEnd(), false);
}

void test_GridIterator::next_ShouldUpdateIndicesAsExpected_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");
    QTest::addColumn<int>("callsToNext");
    QTest::addColumn<int>("expectedRow");
    QTest::addColumn<int>("expectedColumn");

    QTest::newRow("1 row") << 1 << 4 << 2 << 0 << 2;
    QTest::newRow("1 column") << 4 << 1 << 2 << 2 << 0;
    QTest::newRow("end of row") << 4 << 3 << 5 << 1 << 2;
    QTest::newRow("beginning of row") << 4 << 3 << 6 << 2 << 0;
    QTest::newRow("big grid") << 9 << 6 << 40 << 6 << 4;
}

void test_GridIterator::next_ShouldUpdateIndicesAsExpected()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, callsToNext);
    QFETCH(int, expectedRow);
    QFETCH(int, expectedColumn);

    GridIterator it(rows, columns);
    for (int i = 0; i < callsToNext; i++)
    {
        it.next();
    }

    QCOMPARE(it.row(), expectedRow);
    QCOMPARE(it.column(), expectedColumn);
}

void test_GridIterator::isAtEnd_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<int>("columns");
    QTest::addColumn<int>("callsToNext");
    QTest::addColumn<bool>("expectedIsAtEnd");

    QTest::newRow("1 row, last") << 1 << 4 << 3 << false;
    QTest::newRow("1 row, after last") << 1 << 4 << 4 << true;
    QTest::newRow("1 column, last") << 4 << 1 << 3 << false;
    QTest::newRow("1 column, after last") << 1 << 4 << 4 << true;
    QTest::newRow("last") << 6 << 6 << 35 << false;
    QTest::newRow("after last") << 6 << 6 << 36 << true;
    QTest::newRow("way after last") << 6 << 6 << 100 << true;
}

void test_GridIterator::isAtEnd_ShouldReturnExpectedValue()
{
    QFETCH(int, rows);
    QFETCH(int, columns);
    QFETCH(int, callsToNext);
    QFETCH(bool, expectedIsAtEnd);

    GridIterator it(rows, columns);
    for (int i = 0; i < callsToNext; i++)
    {
        it.next();
    }

    QCOMPARE(it.isAtEnd(), expectedIsAtEnd);
}

DECLARE_TEST(test_GridIterator)

#include "test_griditerator.moc"
