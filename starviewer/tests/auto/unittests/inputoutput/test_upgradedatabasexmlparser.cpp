#include "autotest.h"

#include <QList>

#include "upgradedatabaserevisioncommands.h"
#include "upgradedatabasexmlparser.h"

using namespace udg;

class test_UpgradeDatabaseXMLParser : public QObject {
Q_OBJECT

private slots:
    void getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnValidValue_data();
    void getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnValidValue();

    void getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnInvalidValue_data();
    void getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnInvalidValue();

    void getUpgradeDatabaseRevisionCommands_ShouldReturnUpgradeDatabaseRevisionCommands_data();
    void getUpgradeDatabaseRevisionCommands_ShouldReturnUpgradeDatabaseRevisionCommands();

private:
    QString getUpgradeDatabaseXMLTest();
    };

Q_DECLARE_METATYPE(UpgradeDatabaseRevisionCommands)

void test_UpgradeDatabaseXMLParser::getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnValidValue_data()
{
    QTest::addColumn<QString>("xmlData");
    QTest::addColumn<int>("databaseRevisionResult");

    QTest::newRow("Valid UpgradeDatabaseRevision XML") << getUpgradeDatabaseXMLTest()  << 6516;
}

void test_UpgradeDatabaseXMLParser::getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnValidValue()
{
    QFETCH(QString, xmlData);
    QFETCH(int, databaseRevisionResult);

    UpgradeDatabaseXMLParser upgradeDatabaseXMLParser(xmlData);

    QCOMPARE(upgradeDatabaseXMLParser.getMinimumDatabaseRevisionRequiredToUpgrade(), databaseRevisionResult);
}

void test_UpgradeDatabaseXMLParser::getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnInvalidValue_data()
{
    QTest::addColumn<QString>("xmlData");

    QTest::newRow("Invalid UpgradeDatabaseRevision XML") << "";
}

void test_UpgradeDatabaseXMLParser::getMinimumDatabaseRevisionRequiredToUpgrade_ShouldReturnInvalidValue()
{
    QFETCH(QString, xmlData);

    UpgradeDatabaseXMLParser upgradeDatabaseXMLParser(xmlData);

    QCOMPARE(upgradeDatabaseXMLParser.getMinimumDatabaseRevisionRequiredToUpgrade(), -1);
}

void test_UpgradeDatabaseXMLParser::getUpgradeDatabaseRevisionCommands_ShouldReturnUpgradeDatabaseRevisionCommands_data()
{
    QTest::addColumn<QString>("xmlData");
    QTest::addColumn<int>("currentDatabaseRevision");
    QTest::addColumn<UpgradeDatabaseRevisionCommands>("result");

    QStringList sqlUpgradeCommands;
    sqlUpgradeCommands.append("ALTER TABLE STUDY ADD COLUMN InstanceUID TEXT");
    sqlUpgradeCommands.append("ALTER TABLE STUDY ADD COLUMN RetrievedPACSIP TEXT");
    sqlUpgradeCommands.append("ALTER TABLE STUDY ADD COLUMN RetrievedPACSQueryPort INTEGER");

    UpgradeDatabaseRevisionCommands upgradeDatabaseRevisionCommands;
    upgradeDatabaseRevisionCommands.setSqlUpgradeCommands(sqlUpgradeCommands);
    upgradeDatabaseRevisionCommands.setUpgradeToDatabaseRevision(7574);

    QTest::newRow("Valid UpgradeDatabaseRevision XML") << getUpgradeDatabaseXMLTest() << 7200 << upgradeDatabaseRevisionCommands;
}

void test_UpgradeDatabaseXMLParser::getUpgradeDatabaseRevisionCommands_ShouldReturnUpgradeDatabaseRevisionCommands()
{
    QFETCH(QString, xmlData);
    QFETCH(int, currentDatabaseRevision);
    QFETCH(UpgradeDatabaseRevisionCommands, result);

    UpgradeDatabaseRevisionCommands upgradeDatabaseRevisionCommands = UpgradeDatabaseXMLParser(xmlData).getUpgradeDatabaseRevisionCommands(currentDatabaseRevision);

    QCOMPARE(upgradeDatabaseRevisionCommands.getSqlUpgradeCommands().count(), result.getSqlUpgradeCommands().count());

    QCOMPARE(upgradeDatabaseRevisionCommands == result, true);
}

QString test_UpgradeDatabaseXMLParser::getUpgradeDatabaseXMLTest()
{
    QString upgradeDatabaseXMLTest;

    upgradeDatabaseXMLTest =  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    upgradeDatabaseXMLTest += "<upgradeDatabase minimumDatabaseRevisionRequired=\"6516\">";
    upgradeDatabaseXMLTest +=    "<upgradeDatabaseToRevision updateToRevision=\"7000\">";
    upgradeDatabaseXMLTest +=       "<upgradeCommand>ALTER TABLE STUDY ADD COLUMN PatientOrientation TEXT</upgradeCommand>";
    upgradeDatabaseXMLTest +=   "</upgradeDatabaseToRevision>";
    upgradeDatabaseXMLTest +=   "<upgradeDatabaseToRevision updateToRevision=\"7574\">";
    upgradeDatabaseXMLTest +=       "<upgradeCommand>ALTER TABLE STUDY ADD COLUMN RetrievedPACSIP TEXT</upgradeCommand>";
    upgradeDatabaseXMLTest +=       "<upgradeCommand>ALTER TABLE STUDY ADD COLUMN RetrievedPACSQueryPort INTEGER</upgradeCommand>";
    upgradeDatabaseXMLTest +=   "</upgradeDatabaseToRevision>";
    upgradeDatabaseXMLTest +=    "<upgradeDatabaseToRevision updateToRevision=\"7300\">";
    upgradeDatabaseXMLTest +=       "<upgradeCommand>ALTER TABLE STUDY ADD COLUMN InstanceUID TEXT</upgradeCommand>";
    upgradeDatabaseXMLTest +=   "</upgradeDatabaseToRevision>";
    upgradeDatabaseXMLTest += "</upgradeDatabase>";

    return upgradeDatabaseXMLTest;
}

DECLARE_TEST(test_UpgradeDatabaseXMLParser)

#include "test_upgradedatabasexmlparser.moc"

