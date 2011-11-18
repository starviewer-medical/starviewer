#include "autotest.h"

#include "applicationcommandlineoptions.h"

using namespace udg;

class test_ApplicationCommandLineOptions : public QObject {
Q_OBJECT
private slots:
    void getSynopsis_ReturnsExpectedValues_data();
    void getSynopsis_ReturnsExpectedValues();
    
    void addOption_ReturnsExpectedValues_data();
    void addOption_ReturnsExpectedValues();
    
    void parseArgumentList_ReturnsExpectedValues_data();
    void parseArgumentList_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(CommandLineOption)
Q_DECLARE_METATYPE(QList<CommandLineOption>)
Q_DECLARE_METATYPE(QList<bool>)

void test_ApplicationCommandLineOptions::getSynopsis_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("appName");
    QTest::addColumn<QList<CommandLineOption> >("optionsList");
    QTest::addColumn<QString>("expectedSynopsis");

    QString appName = "applicationX";
    QList<CommandLineOption> commands;
    QString expectedSynopsis;
    
    CommandLineOption option1("option1", false, "Description of option 1");
    
    commands << option1;
    
    expectedSynopsis = "Synopsis:\n" + appName + " [" + option1.toString(false) + "]";
    expectedSynopsis += "\n\nOptions:\n" + option1.toString(true) + "\n\n";
    
    QTest::newRow("single element list") << appName << commands << expectedSynopsis;

    CommandLineOption option2("option2", true, "Description of option 2 which needs an argument");
    
    commands << option2;
    
    expectedSynopsis = "Synopsis:\n" + appName + " [" + option1.toString(false) + "] [" + option2.toString(false) + "]";
    expectedSynopsis += "\n\nOptions:\n" + option1.toString(true) + "\n\n" + option2.toString(true) + "\n\n";
    
    QTest::newRow("two elements lists") << appName << commands << expectedSynopsis;
}

void test_ApplicationCommandLineOptions::getSynopsis_ReturnsExpectedValues()
{
    QFETCH(QString, appName);
    QFETCH(QList<CommandLineOption>, optionsList);
    QFETCH(QString, expectedSynopsis);

    ApplicationCommandLineOptions commandLine(appName);

    foreach (const CommandLineOption &option, optionsList)
    {
        commandLine.addOption(option);
    }
    
    QCOMPARE(commandLine.getSynopsis(), expectedSynopsis);
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

    ApplicationCommandLineOptions appOptions("appName");

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

    ApplicationCommandLineOptions commandLine("appName");
    commandLine.addOption(option);
    
    QCOMPARE(commandLine.parseArgumentList(argumentsList), expectedResult);
}

DECLARE_TEST(test_ApplicationCommandLineOptions)

#include "test_applicationcommandlineoptions.moc"