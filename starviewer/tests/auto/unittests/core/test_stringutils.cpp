/*@
    "name": "test_StringUtils",
    "requirements": []
 */

#include "autotest.h"
#include "stringutils.h"

using namespace udg::StringUtils;

class test_StringUtils : public QObject {
    Q_OBJECT

private slots:
    void findCommonPattern_ShouldReturnExpectedValue_data();
    void findCommonPattern_ShouldReturnExpectedValue();

};

void test_StringUtils::findCommonPattern_ShouldReturnExpectedValue_data()
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

void test_StringUtils::findCommonPattern_ShouldReturnExpectedValue()
{
    QFETCH(QString, string1);
    QFETCH(QString, string2);
    QFETCH(QString, patternString);
    QFETCH(QString, expectedValue);

    QCOMPARE(findCommonPattern(string1, string2, patternString), expectedValue);
}

DECLARE_TEST(test_StringUtils)

#include "test_stringutils.moc"
