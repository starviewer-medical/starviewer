#include "autotest.h"

#include "applicationcommandlineoptions.h"

using namespace udg;

class test_ApplicationCommandLineOptions : public QObject {
Q_OBJECT
private slots:
    void addOption_SetsDataCorrectly_data();
    void addOption_SetsDataCorrectly();

    void parseArgumentList_ReturnsExpectedValues_data();
    void parseArgumentList_ReturnsExpectedValues();
};

void test_ApplicationCommandLineOptions::addOption_SetsDataCorrectly_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("requiresArgument");
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOptionsDescription");

    QString name1 = "option1";
    QString description1 = "Description of option 1";
    QTest::newRow("option1, no argument, with description") << name1 << false << description1 << "-option1\n\tDescription of option 1\n";

    QString name2 = "option2";
    QString description2 = "Description of option 2 which needs an argument";
    QTest::newRow("option2, argument, with description") << name2 << true << description2 << "-option2 <value>\n\tDescription of option 2 which needs an argument\n";
}

void test_ApplicationCommandLineOptions::addOption_SetsDataCorrectly()
{
    QFETCH(QString, name);
    QFETCH(bool, requiresArgument);
    QFETCH(QString, description);
    QFETCH(QString, expectedOptionsDescription);

    ApplicationCommandLineOptions commandLine;
    commandLine.addOption(name, requiresArgument, description);
    
    QCOMPARE(commandLine.getOptionsDescription(), expectedOptionsDescription);
}

void test_ApplicationCommandLineOptions::parseArgumentList_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("optionName");
    QTest::addColumn<bool>("requiresArgument");
    QTest::addColumn<QStringList>("argumentsList");
    QTest::addColumn<bool>("expectedResult");

    QStringList arguments;
    
    arguments << "starviewer.exe" << "-option1"; 
    QTest::newRow("valid option with no required argument (ok)") << "option1" << false << arguments << true;
    QTest::newRow("option does not exist (!ok)") << "someOption" << false << arguments << false;
    QTest::newRow("valid option requires an argument (!ok)") << "option1" << true << arguments << false;

    arguments << "12345";
    QTest::newRow("valid option with required argument (ok)") << "option1" << true << arguments << true;
    QTest::newRow("valid option does not require argument followed by an argument [ignored] (ok)") << "option1" << false << arguments << true;
}

void test_ApplicationCommandLineOptions::parseArgumentList_ReturnsExpectedValues()
{
    QFETCH(QString, optionName);
    QFETCH(bool, requiresArgument);
    QFETCH(QStringList, argumentsList);
    QFETCH(bool, expectedResult);

    ApplicationCommandLineOptions commandLine;
    commandLine.addOption(optionName, requiresArgument, QString());
    
    QCOMPARE(commandLine.parseArgumentList(argumentsList), expectedResult);
}

DECLARE_TEST(test_ApplicationCommandLineOptions)

#include "test_applicationcommandlineoptions.moc"