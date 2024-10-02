/*@
    "name": "test_Utils",
    "requirements": []
 */

#include "autotest.h"
#include "utils.h"

using namespace udg::Utils;

class test_Utils : public QObject {
    Q_OBJECT

private slots:
    void findCommonPattern_ShouldReturnExpectedValue_data();
    void findCommonPattern_ShouldReturnExpectedValue();

    void compareUintVectors_ShouldReturnExpectedValue_data();
    void compareUintVectors_ShouldReturnExpectedValue();

    void compareUids_ShouldReturnExpectedValue_data();
    void compareUids_ShouldReturnExpectedValue();
};

void test_Utils::findCommonPattern_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QString>("string1");
    QTest::addColumn<QString>("string2");
    QTest::addColumn<QString>("patternString");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("both empty") << "" << "" << "*" << "";
    QTest::newRow("left empty") << "" << "bbbb" << "*" << "*";
    QTest::newRow("right empty") << "aaaa" << "" << "*" << "*";
    QTest::newRow("equal") << "asdf" << "asdf" << "*" << "asdf";
    QTest::newRow("different ending") << "abcdef" << "abcxyz" << "..." << "abc...";
    QTest::newRow("different beginning") << "North Korea" << "South Korea" << "^" << "^th Korea";
    QTest::newRow("different center") << "I'll be back" << "I'll go back" << "*" << "I'll * back";
    QTest::newRow("different beginning and ending") << "ab_cd" << "12_34" << "*" << "*";
    QTest::newRow("left substring") << "Monday" << "Monday, Tuesday" << "*" << "Monday*";
    QTest::newRow("right substring") << "November, December" << "December" << "*" << "*December";
}

void test_Utils::findCommonPattern_ShouldReturnExpectedValue()
{
    QFETCH(QString, string1);
    QFETCH(QString, string2);
    QFETCH(QString, patternString);
    QFETCH(QString, expectedValue);

    QCOMPARE(findCommonPattern(string1, string2, patternString), expectedValue);
}

void test_Utils::compareUintVectors_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QVector<uint>>("vector1");
    QTest::addColumn<QVector<uint>>("vector2");
    QTest::addColumn<int>("expectedResult");

    using V = QVector<uint>;

    QTest::newRow("empty vectors") << V{} << V{} << 0;
    QTest::newRow("first empty") << V{} << V{1} << -1;
    QTest::newRow("second empty") << V{1} << V{} << 1;
    QTest::newRow("first smaller") << V{1, 2, 3} << V{1, 2, 4} << -1;
    QTest::newRow("equal") << V{1, 2, 3} << V{1, 2, 3} << 0;
    QTest::newRow("second smaller") << V{1, 2, 3} << V{1, 2, 2} << 1;
    QTest::newRow("first is prefix") << V{1, 2, 3} << V{1, 2, 3, 4} << -1;
    QTest::newRow("second is prefix") << V{1, 2, 3, 4} << V{1, 2, 3} << 1;
    QTest::newRow("different sizes but not prefix") << V{1, 3, 3, 7} << V{1, 3, 1, 2} << 1;
    QTest::newRow("positive or negative does not imply +1 or -1") << V{1, 2, 3} << V{1} << 2;
}

void test_Utils::compareUintVectors_ShouldReturnExpectedValue()
{
    QFETCH(QVector<uint>, vector1);
    QFETCH(QVector<uint>, vector2);
    QFETCH(int, expectedResult);

    QCOMPARE(compareUintVectors(vector1, vector2), expectedResult);
}

void test_Utils::compareUids_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QString>("uid1");
    QTest::addColumn<QString>("uid2");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("empty uids") << "" << "" << 0;
    QTest::newRow("first empty") << "" << "1" << -1;
    QTest::newRow("second empty") << "1" << "" << 1;
    QTest::newRow("first smaller") << "1.2.3" << "1.2.4" << -1;
    QTest::newRow("equal") << "1.2.3" << "1.2.3" << 0;
    QTest::newRow("second smaller") << "1.2.3" << "1.2.2" << 1;
    QTest::newRow("first is prefix") << "1.2.3" << "1.2.3.4" << -1;
    QTest::newRow("second is prefix") << "1.2.3.4" << "1.2.3" << 1;
    QTest::newRow("different sizes but not prefix") << "1.3.3.7" << "1.3.1.2" << 1;
    QTest::newRow("positive or negative does not imply +1 or -1") << "1.2.3" << "1" << 2;
}

void test_Utils::compareUids_ShouldReturnExpectedValue()
{
    QFETCH(QString, uid1);
    QFETCH(QString, uid2);
    QFETCH(int, expectedResult);

    QCOMPARE(compareUids(uid1, uid2), expectedResult);
}

DECLARE_TEST(test_Utils)

#include "test_utils.moc"

