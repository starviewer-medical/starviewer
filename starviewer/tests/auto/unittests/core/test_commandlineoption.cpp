#include "autotest.h"

#include "commandlineoption.h"

using namespace udg;

class test_CommandLineOption : public QObject {
Q_OBJECT
private slots:
    
    void toString_ReturnsExpectedValues_data();
    void toString_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(CommandLineOption)

void test_CommandLineOption::toString_ReturnsExpectedValues_data()
{
    QTest::addColumn<CommandLineOption>("option");
    QTest::addColumn<bool>("withDescription");
    QTest::addColumn<QString>("expectedString");
    
    CommandLineOption option1("option1", false, "Description of option 1");
    QTest::newRow("option1, no argument, with description") << option1 << true << "-option1 \tDescription of option 1";
    QTest::newRow("option1, no argument, without description") << option1 << false << "-option1";

    CommandLineOption option2("option2", true, "Description of option 2 which needs an argument");
    QTest::newRow("option2, argument, with description") << option2 << true << "-option2 value \tDescription of option 2 which needs an argument";
    QTest::newRow("option2, argument, without description") << option2 << false << "-option2 value";
}

void test_CommandLineOption::toString_ReturnsExpectedValues()
{
    QFETCH(CommandLineOption, option);
    QFETCH(bool, withDescription);
    QFETCH(QString, expectedString);

    QCOMPARE(option.toString(withDescription), expectedString);
}

DECLARE_TEST(test_CommandLineOption)

#include "test_commandlineoption.moc"