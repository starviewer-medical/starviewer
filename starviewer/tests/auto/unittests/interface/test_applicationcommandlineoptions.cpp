#include "autotest.h"

#include "applicationcommandlineoptions.h"

using namespace udg;

class test_ApplicationCommandLineOptions : public QObject {
Q_OBJECT
private slots:
    void addOption_SetsDataCorrectly_data();
    void addOption_SetsDataCorrectly();

    void addOption_ReturnsExpectedValues_data();
    void addOption_ReturnsExpectedValues();
    
    void parseArgumentList_ReturnsExpectedValues_data();
    void parseArgumentList_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(CommandLineOption)
Q_DECLARE_METATYPE(QList<CommandLineOption>)
Q_DECLARE_METATYPE(QList<bool>)

void test_ApplicationCommandLineOptions::addOption_SetsDataCorrectly_data()
{
    QTest::addColumn<CommandLineOption>("option");
    QTest::addColumn<QString>("expectedOptionsDescription");

    CommandLineOption option1("option1", false, "Description of option 1");
    QTest::newRow("option1, no argument, with description") << option1 << "-option1\n\tDescription of option 1\n";

    CommandLineOption option2("option2", true, "Description of option 2 which needs an argument");
    QTest::newRow("option2, argument, with description") << option2 << "-option2 <value>\n\tDescription of option 2 which needs an argument\n";
}

void test_ApplicationCommandLineOptions::addOption_SetsDataCorrectly()
{
    QFETCH(CommandLineOption, option);
    QFETCH(QString, expectedOptionsDescription);

    ApplicationCommandLineOptions commandLine;
    commandLine.addOption(option);
    
    QCOMPARE(commandLine.getOptionsDescription(), expectedOptionsDescription);
}

void test_ApplicationCommandLineOptions::addOption_ReturnsExpectedValues_data()
{
    QTest::addColumn<QList<CommandLineOption> >("optionsList");
    QTest::addColumn<QList<bool> >("expectedResults");

    QList<CommandLineOption> commands;
    QList<bool> results;
    
    commands << CommandLineOption("option1", true, "description1");
    commands << CommandLineOption("option1", false, "description1");

    results << true << false;
    QTest::newRow("Repeated option") << commands << results;
}

void test_ApplicationCommandLineOptions::addOption_ReturnsExpectedValues()
{
    QFETCH(QList<CommandLineOption>, optionsList);
    QFETCH(QList<bool>, expectedResults);

    ApplicationCommandLineOptions appOptions;

    for (int i = 0; i < optionsList.count(); ++i)
    {
        QCOMPARE(appOptions.addOption(optionsList.at(i)), expectedResults.at(i));
    }
}

void test_ApplicationCommandLineOptions::parseArgumentList_ReturnsExpectedValues_data()
{
    QTest::addColumn<CommandLineOption>("option");
    QTest::addColumn<QStringList>("argumentsList");
    QTest::addColumn<bool>("expectedResult");

    QStringList arguments;
    
    arguments << "starviewer.exe" << "-option1"; 
    QTest::newRow("valid option with no required argument (ok)") << CommandLineOption("option1", false, "") << arguments << true;
    QTest::newRow("option does not exist (!ok)") << CommandLineOption("someOption", false, "") << arguments << false;
    QTest::newRow("valid option requires an argument (!ok)") << CommandLineOption("option1", true, "") << arguments << false;

    arguments << "12345";
    QTest::newRow("valid option with required argument (ok)") << CommandLineOption("option1", true, "") << arguments << true;
    QTest::newRow("valid option does not require argument followed by an argument [ignored] (ok)") << CommandLineOption("option1", false, "") << arguments << true;
}

void test_ApplicationCommandLineOptions::parseArgumentList_ReturnsExpectedValues()
{
    QFETCH(CommandLineOption, option);
    QFETCH(QStringList, argumentsList);
    QFETCH(bool, expectedResult);

    ApplicationCommandLineOptions commandLine;
    commandLine.addOption(option);
    
    QCOMPARE(commandLine.parseArgumentList(argumentsList), expectedResult);
}

DECLARE_TEST(test_ApplicationCommandLineOptions)

#include "test_applicationcommandlineoptions.moc"