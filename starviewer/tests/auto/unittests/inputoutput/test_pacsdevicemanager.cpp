/*@
    "name": "test_PacsDeviceManager",
    "requirements": ["archive.dimse", "archive.wado"]
 */

#include "autotest.h"
#include "pacsdevicemanager.h"

#include "inputoutputsettings.h"
#include "pacsdevice.h"
#include "pacsdevicetesthelper.h"
#include "testingsettings.h"

using namespace udg;
using namespace testing;

class test_PacsDeviceManager : public QObject {
Q_OBJECT

private slots:
    void addPacs_ShouldAddThePacs_data();
    void addPacs_ShouldAddThePacs();

    void addPacs_ShouldNotAddThePacs_data();
    void addPacs_ShouldNotAddThePacs();

    void updatePacs_ShouldUpdateThePacs_data();
    void updatePacs_ShouldUpdateThePacs();

    void updatePacs_ShouldNotChangeAnything_data();
    void updatePacs_ShouldNotChangeAnything();

    void deletePacs_ShouldDeleteThePacs_data();
    void deletePacs_ShouldDeleteThePacs();

    void deletePacs_ShouldNotChangeAnything_data();
    void deletePacs_ShouldNotChangeAnything();

    void getPacsList_ShouldReturnPacsAccordingToFilter_data();
    void getPacsList_ShouldReturnPacsAccordingToFilter();

    void getPacsDeviceById_ShouldReturnExpectedPacsDevice_data();
    void getPacsDeviceById_ShouldReturnExpectedPacsDevice();

    void removeDuplicatePacsFromList_ShouldReturnListWithoutDuplicatePACS_data();
    void removeDuplicatePacsFromList_ShouldReturnListWithoutDuplicatePACS();

    void isAddedSamePacsDeviceInList_ShouldReturnIsSamePacsDeviceIsAddedToList_data();
    void isAddedSamePacsDeviceInList_ShouldReturnIsSamePacsDeviceIsAddedToList();

    void cleanupTestCase();
};

Q_DECLARE_METATYPE(PacsDevice)
Q_DECLARE_METATYPE(PacsDeviceManager::PacsFilter)
Q_DECLARE_METATYPE(QList<PacsDevice>)
Q_DECLARE_METATYPE(TestingSettings)

void test_PacsDeviceManager::addPacs_ShouldAddThePacs_data()
{
    QTest::addColumn<TestingSettings>("settingsBefore");
    QTest::addColumn<PacsDevice>("pacsDevice");
    QTest::addColumn<QString>("expectedId");
    QTest::addColumn<TestingSettings>("settingsAfter");

    TestingSettings settings0, settings1, settings2, settings3, settings4, settings5;

    {
        PacsDevice pacsDevice;
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("A");
        pacsDevice.setAddress("1.2.3.4");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1234);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(1111);
        pacsDevice.setInstitution("I0");
        pacsDevice.setLocation("L0");
        pacsDevice.setDescription("D0");

        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        settings1.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("First PACS") << settings0 << pacsDevice << "0" << settings1;
    }

    settings2 = settings1;

    {
        PacsDevice pacsDevice;
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(QUrl("https://example.com/wado"));
        pacsDevice.setInstitution("I1");
        pacsDevice.setLocation("L1");
        pacsDevice.setDescription("D1");

        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        settings2.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Second PACS (last id + 1)") << settings1 << pacsDevice << "1" << settings2;
    }

    settings3 = settings2;

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("10");
        pacsDevice.setType(PacsDevice::Type::WadoUriDimse);
        pacsDevice.setAETitle("^_^");
        pacsDevice.setAddress("0.0.0.1");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(11111);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(1312);
        pacsDevice.setBaseUri(QUrl("https://example.com/wado-uri"));
        pacsDevice.setLocation("L2");

        Settings::SettingsListItemType item;
        item["ID"] = "2";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        settings3.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Input id is ignored") << settings2 << pacsDevice << "2" << settings3;
    }

    settings4 = settings3;

    {
        PacsDevice pacsDevice;
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("A");
        pacsDevice.setAddress("1.2.3.4");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(12345);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(1111);
        pacsDevice.setInstitution("I0");
        pacsDevice.setLocation("L0");
        pacsDevice.setDescription("D0");

        Settings::SettingsListItemType item;
        item["ID"] = "3";
        item["Type"] = "DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        settings4.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("DIMSE repeated everything equal except Q/R port") << settings3 << pacsDevice << "3" << settings4;
    }

    settings5 = settings4;

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("10");
        pacsDevice.setType(PacsDevice::Type::WadoUriDimse);
        pacsDevice.setAETitle("^_^");
        pacsDevice.setAddress("0.0.0.1");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1111);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(1312);
        pacsDevice.setBaseUri(QUrl("https://example.com:1312/wado-uri"));
        pacsDevice.setLocation("L2");

        Settings::SettingsListItemType item;
        item["ID"] = "4";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        settings5.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Hybrid PACS with some key field different") << settings4 << pacsDevice << "4" << settings5;
    }
}

void test_PacsDeviceManager::addPacs_ShouldAddThePacs()
{
    QFETCH(TestingSettings, settingsBefore);
    QFETCH(PacsDevice, pacsDevice);
    QFETCH(QString, expectedId);
    QFETCH(TestingSettings, settingsAfter);

    Settings::setStaticTestingSettings(&settingsBefore);

    bool result = PacsDeviceManager::addPacs(pacsDevice);

    QCOMPARE(result, true);
    QCOMPARE(pacsDevice.getID(), expectedId);
    QCOMPARE(settingsBefore, settingsAfter);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::addPacs_ShouldNotAddThePacs_data()
{
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<PacsDevice>("pacsDevice");

    TestingSettings testingSettings;
    {
        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "DIMSE";
        item["AETitle"] = "||*||";
        item["PacsHostname"] = "cat";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 1714;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2017;
        testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = "https://127.0.0.1:9996/wado/";
        testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "2";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = "WUD";
        item["PacsHostname"] = "4.7.89.111";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 1234;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 5678;
        item["BaseUri"] = "https://127.0.0.1:9999";
        testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }

    {
        PacsDevice pacsDevice;
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("||*||");
        pacsDevice.setAddress("cat");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1714);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(2017);

        QTest::newRow("Same DIMSE") << testingSettings << pacsDevice;
    }

    QTest::newRow("Same DIMSE Q/R different Store") << testingSettings << PACSDeviceTestHelper::createDimsePacsDevice("3", "||*||", "cat", 1714);

    QTest::newRow("Same WADO") << testingSettings << PACSDeviceTestHelper::createWadoPacsDevice("4", "https://127.0.0.1:9996/wado/");

    QTest::newRow("Same hybrid") << testingSettings
                                 << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("5", "WUD", "4.7.89.111", 1234, "https://127.0.0.1:9999");
}

void test_PacsDeviceManager::addPacs_ShouldNotAddThePacs()
{
    QFETCH(TestingSettings, testingSettings);
    QFETCH(PacsDevice, pacsDevice);

    Settings::setStaticTestingSettings(&testingSettings);

    TestingSettings settingsCopy(testingSettings);
    PacsDevice pacsCopy(pacsDevice);

    bool result = PacsDeviceManager::addPacs(pacsDevice);

    QCOMPARE(result, false);
    QCOMPARE(pacsDevice, pacsCopy);
    QCOMPARE(testingSettings, settingsCopy);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::updatePacs_ShouldUpdateThePacs_data()
{
    QTest::addColumn<TestingSettings>("settingsBefore");
    QTest::addColumn<PacsDevice>("pacsDevice");
    QTest::addColumn<QString>("expectedId");
    QTest::addColumn<TestingSettings>("settingsAfter");

    TestingSettings initialSettings;
    {
        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "DIMSE";
        item["AETitle"] = "||*||";
        item["PacsHostname"] = "cat";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 1714;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2017;
        item["Institution"] = "I0";
        item["Location"] = "L0";
        item["Description"] = "D0";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = "https://127.0.0.1:9996/wado/";
        item["Institution"] = "I1";
        item["Location"] = "L1";
        item["Description"] = "D1";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "2";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = "||*||";
        item["PacsHostname"] = "cat";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 1714;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2017;
        item["BaseUri"] = "https://example.com/wado-uri/";
        item["Institution"] = "I2";
        item["Location"] = "L2";
        item["Description"] = "D2";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("0");
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("//*//");
        pacsDevice.setAddress("1.7.1.4");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1207);
        pacsDevice.setStoreServiceEnabled(false);
        pacsDevice.setStoreServicePort(2017);
        pacsDevice.setInstitution("Consell");
        pacsDevice.setLocation("UE");
        pacsDevice.setDescription("PE");

        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.setListItem(0, InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Update DIMSE") << initialSettings << pacsDevice << "0" << updatedSettings;
    }

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("1");
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(QUrl("https://localhost:9999/"));
        pacsDevice.setInstitution("I1");
        pacsDevice.setLocation("L1");
        pacsDevice.setDescription("Ç");

        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.setListItem(1, InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Update WADO") << initialSettings << pacsDevice << "1" << updatedSettings;
    }

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("2");
        pacsDevice.setType(PacsDevice::Type::WadoUriDimse);
        pacsDevice.setAETitle("//*//");
        pacsDevice.setAddress("1.7.1.4");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1207);
        pacsDevice.setStoreServiceEnabled(false);
        pacsDevice.setStoreServicePort(2017);
        pacsDevice.setBaseUri(QUrl("https://localhost:9999/"));
        pacsDevice.setInstitution("Consell");
        pacsDevice.setLocation("UE");
        pacsDevice.setDescription("PE");

        Settings::SettingsListItemType item;
        item["ID"] = "2";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = pacsDevice.getAETitle();
        item["PacsHostname"] = pacsDevice.getAddress();
        item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
        item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
        item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
        item["StoreServicePort"] = pacsDevice.getStoreServicePort();
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.setListItem(2, InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Update hybrid") << initialSettings << pacsDevice << "2" << updatedSettings;
    }

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("1");
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(QUrl("https://nyan.cat/nyan"));
        pacsDevice.setInstitution("Nyan");
        pacsDevice.setLocation("Nyan nyan");
        pacsDevice.setDescription("D0");

        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.setListItem(1, InputOutputSettings::PacsListConfigurationSectionName, item);

        QTest::newRow("Update from DIMSE to WADO") << initialSettings << pacsDevice << "1" << updatedSettings;
    }

    {
        TestingSettings alternateInitialSettings(initialSettings);
        auto list = alternateInitialSettings.getList(InputOutputSettings::PacsListConfigurationSectionName);
        list[0]["ID"] = "1";
        list[1]["ID"] = "2";
        alternateInitialSettings.setList(InputOutputSettings::PacsListConfigurationSectionName, list);

        PacsDevice pacsDevice;
        pacsDevice.setID("2");
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(QUrl("https://nyan.cat/nyan"));
        pacsDevice.setInstitution("Nyan");
        pacsDevice.setLocation("L1");
        pacsDevice.setDescription("Nyan nyan nyan nyan");

        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = pacsDevice.getBaseUri();
        item["Institution"] = pacsDevice.getInstitution();
        item["Location"] = pacsDevice.getLocation();
        item["Description"] = pacsDevice.getDescription();

        TestingSettings updatedSettings(alternateInitialSettings);
        auto updatedList = updatedSettings.getList(InputOutputSettings::PacsListConfigurationSectionName);
        updatedList[0]["ID"] = "0";
        updatedList[1] = item;
        updatedSettings.setList(InputOutputSettings::PacsListConfigurationSectionName, updatedList);

        QTest::newRow("Change ids") << alternateInitialSettings << pacsDevice << "1" << updatedSettings;
    }
}

void test_PacsDeviceManager::updatePacs_ShouldUpdateThePacs()
{
    QFETCH(TestingSettings, settingsBefore);
    QFETCH(PacsDevice, pacsDevice);
    QFETCH(QString, expectedId);
    QFETCH(TestingSettings, settingsAfter);

    Settings::setStaticTestingSettings(&settingsBefore);

    PacsDeviceManager::updatePacs(pacsDevice);

    QCOMPARE(pacsDevice.getID(), expectedId);
    QCOMPARE(settingsBefore, settingsAfter);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::updatePacs_ShouldNotChangeAnything_data()
{
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<PacsDevice>("pacsDevice");

    TestingSettings testingSettings;

    {
        PacsDevice pacsDevice;
        pacsDevice.setID("0");
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("...");
        pacsDevice.setAddress("asdf");
        pacsDevice.setQueryRetrieveServiceEnabled(true);
        pacsDevice.setQueryRetrieveServicePort(1111);
        pacsDevice.setStoreServiceEnabled(false);
        pacsDevice.setStoreServicePort(2222);

        QTest::newRow("Empty settings") << testingSettings << pacsDevice;
    }

    {
        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "WADO";
        item["BaseUri"] = "https://127.0.0.1:9996/wado/";
        testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

        PacsDevice pacsDevice;
        pacsDevice.setID("3");
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(QUrl("https://aaaaaaaa.com"));

        QTest::newRow("Inexistent id") << testingSettings << pacsDevice;
    }
}

void test_PacsDeviceManager::updatePacs_ShouldNotChangeAnything()
{
    QFETCH(TestingSettings, testingSettings);
    QFETCH(PacsDevice, pacsDevice);

    Settings::setStaticTestingSettings(&testingSettings);

    TestingSettings settingsCopy(testingSettings);
    PacsDevice pacsCopy(pacsDevice);

    PacsDeviceManager::updatePacs(pacsDevice);

    QCOMPARE(pacsDevice, pacsCopy);
    QCOMPARE(testingSettings, settingsCopy);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::deletePacs_ShouldDeleteThePacs_data()
{
    QTest::addColumn<TestingSettings>("settingsBefore");
    QTest::addColumn<QString>("pacsId");
    QTest::addColumn<TestingSettings>("settingsAfter");

    TestingSettings initialSettings;
    {
        Settings::SettingsListItemType item;
        item["ID"] = "0";
        item["Type"] = "DIMSE";
        item["AETitle"] = "||*||";
        item["PacsHostname"] = "cat";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 1714;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2017;
        item["Institution"] = "I0";
        item["Location"] = "L0";
        item["Description"] = "D0";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "1";
        item["Type"] = "WADO";
        item["BaseUri"] = "https://127.0.0.1:9996/wado/";
        item["Institution"] = "I1";
        item["Location"] = "L1";
        item["Description"] = "D1";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }
    {
        Settings::SettingsListItemType item;
        item["ID"] = "2";
        item["Type"] = "WADO-URI+DIMSE";
        item["AETitle"] = "@.@";
        item["PacsHostname"] = "10.6.222.5";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 2446;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2447;
        item["BaseUri"] = "https://127.0.0.1:9996/wado-uri/";
        item["Institution"] = "I2";
        item["Location"] = "L2";
        item["Description"] = "D2";
        initialSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);
    }

    {
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.removeListItem(InputOutputSettings::PacsListConfigurationSectionName, 2);

        QTest::newRow("Delete last (ids not changed)") << initialSettings << "2" << updatedSettings;
    }

    {
        TestingSettings updatedSettings(initialSettings);
        updatedSettings.removeListItem(InputOutputSettings::PacsListConfigurationSectionName, 1);
        auto list = updatedSettings.getList(InputOutputSettings::PacsListConfigurationSectionName);
        list[1]["ID"] = "1";
        updatedSettings.setList(InputOutputSettings::PacsListConfigurationSectionName, list);

        QTest::newRow("Delete middle (ids changed)") << initialSettings << "1" << updatedSettings;
    }
}

void test_PacsDeviceManager::deletePacs_ShouldDeleteThePacs()
{
    QFETCH(TestingSettings, settingsBefore);
    QFETCH(QString, pacsId);
    QFETCH(TestingSettings, settingsAfter);

    Settings::setStaticTestingSettings(&settingsBefore);

    PacsDeviceManager::deletePacs(pacsId);

    QCOMPARE(settingsBefore, settingsAfter);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::deletePacs_ShouldNotChangeAnything_data()
{
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<QString>("pacsId");

    TestingSettings testingSettings;
    Settings::SettingsListItemType item;
    item["ID"] = "0";
    item["Type"] = "WADO";
    item["BaseUri"] = "https://127.0.0.1:9996/wado/";
    testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

    QTest::newRow("Inexistent id") << testingSettings << "5";
}

void test_PacsDeviceManager::deletePacs_ShouldNotChangeAnything()
{
    QFETCH(TestingSettings, testingSettings);
    QFETCH(QString, pacsId);

    Settings::setStaticTestingSettings(&testingSettings);

    TestingSettings settingsCopy(testingSettings);

    PacsDeviceManager::deletePacs(pacsId);

    QCOMPARE(testingSettings, settingsCopy);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::getPacsList_ShouldReturnPacsAccordingToFilter_data()
{
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<PacsDeviceManager::PacsFilter>("filter");
    QTest::addColumn<QList<PacsDevice>>("expectedList");

    TestingSettings settings;
    Settings::setStaticTestingSettings(&settings);  // needed for calls to setDefault

    // DIMSE Q/R No default
    PacsDevice pacs0 = PACSDeviceTestHelper::createDimsePacsDevice("0", "pacs0", "1.0.0.0", 2000);
    PacsDeviceManager::addPacs(pacs0);

    // DIMSE Q/R Default
    PacsDevice pacs1 = PACSDeviceTestHelper::createDimsePacsDevice("1", "pacs1", "1.0.0.1", 2001);
    pacs1.setDefault(true);
    PacsDeviceManager::addPacs(pacs1);

    // DIMSE Q/R Store No default
    PacsDevice pacs2 = PACSDeviceTestHelper::createDimsePacsDevice("2", "pacs2", "1.0.0.2", 2002);
    pacs2.setStoreServiceEnabled(true);
    pacs2.setStoreServicePort(2012);
    PacsDeviceManager::addPacs(pacs2);

    // DIMSE Q/R Store Default
    PacsDevice pacs3 = PACSDeviceTestHelper::createDimsePacsDevice("3", "pacs3", "1.0.0.3", 2003);
    pacs3.setStoreServiceEnabled(true);
    pacs3.setStoreServicePort(2013);
    pacs3.setDefault(true);
    PacsDeviceManager::addPacs(pacs3);

    // DIMSE Store No default
    PacsDevice pacs4 = PACSDeviceTestHelper::createDimsePacsDevice("4", "pacs4", "1.0.0.4", 2004);
    pacs4.setQueryRetrieveServiceEnabled(false);
    pacs4.setStoreServiceEnabled(true);
    pacs4.setStoreServicePort(2014);
    PacsDeviceManager::addPacs(pacs4);

    // DIMSE Store Default
    PacsDevice pacs5 = PACSDeviceTestHelper::createDimsePacsDevice("5", "pacs5", "1.0.0.5", 2005);
    pacs5.setQueryRetrieveServiceEnabled(false);
    pacs5.setStoreServiceEnabled(true);
    pacs5.setStoreServicePort(2015);
    pacs5.setDefault(true);
    PacsDeviceManager::addPacs(pacs5);

    // WADO No default
    PacsDevice pacs6 = PACSDeviceTestHelper::createWadoPacsDevice("6", "http://pacs6.net");
    PacsDeviceManager::addPacs(pacs6);

    // WADO Default
    PacsDevice pacs7 = PACSDeviceTestHelper::createWadoPacsDevice("7", "http://pacs7.net");
    pacs7.setDefault(true);
    PacsDeviceManager::addPacs(pacs7);

    // Hybrid No default
    PacsDevice pacs8 = PACSDeviceTestHelper::createWadoUriDimsePacsDevice("8", "pacs8", "1.0.0.8", 2008, "http://pacs8.net");
    PacsDeviceManager::addPacs(pacs8);

    // Hybrid Default
    PacsDevice pacs9 = PACSDeviceTestHelper::createWadoUriDimsePacsDevice("9", "pacs9", "1.0.0.9", 2009, "http://pacs9.net");
    pacs9.setDefault(true);
    PacsDeviceManager::addPacs(pacs9);

    // Hybrid Store No default
    PacsDevice pacs10 = PACSDeviceTestHelper::createWadoUriDimsePacsDevice("10", "pacs10", "1.0.0.10", 2010, "http://pacs10.net");
    pacs10.setStoreServiceEnabled(true);
    pacs10.setStoreServicePort(2020);
    PacsDeviceManager::addPacs(pacs10);

    // Hybrid Store Default
    PacsDevice pacs11 = PACSDeviceTestHelper::createWadoUriDimsePacsDevice("11", "pacs11", "1.0.0.11", 2011, "http://pacs11.net");
    pacs11.setStoreServiceEnabled(true);
    pacs11.setStoreServicePort(2021);
    pacs11.setDefault(true);
    PacsDeviceManager::addPacs(pacs11);

    Settings::setStaticTestingSettings(nullptr);

    using PDM = PacsDeviceManager;
    using PacsFilter = PDM::PacsFilter;
    using List = QList<PacsDevice>;

    QTest::newRow("Null filter") << settings << PacsFilter(0) << List{};

    QTest::newRow("Can retrieve") << settings << PacsFilter(PDM::CanRetrieve) << List{pacs0, pacs1, pacs2, pacs3, pacs6, pacs7, pacs8, pacs9, pacs10, pacs11};
    QTest::newRow("Can store") << settings << PacsFilter(PDM::CanStore) << List{pacs2, pacs3, pacs4, pacs5, pacs6, pacs7, pacs10, pacs11};
    QTest::newRow("All (can retrieve | can store)") << settings << PacsFilter(PDM::All)
                                                    << List{pacs0, pacs1, pacs2, pacs3, pacs4, pacs5, pacs6, pacs7, pacs8, pacs9, pacs10, pacs11};
    QTest::newRow("Only default") << settings << PacsFilter(PDM::OnlyDefault) << List{};

    QTest::newRow("Can retrieve only default") << settings << PacsFilter(PDM::CanRetrieve | PDM::OnlyDefault) << List{pacs1, pacs3, pacs7, pacs9, pacs11};
    QTest::newRow("Can store only default") << settings << PacsFilter(PDM::CanStore | PDM::OnlyDefault) << List{pacs3, pacs5, pacs7, pacs11};
    QTest::newRow("All only default") << settings << PacsFilter(PDM::All | PDM::OnlyDefault) << List{pacs1, pacs3, pacs5, pacs7, pacs9, pacs11};
}

void test_PacsDeviceManager::getPacsList_ShouldReturnPacsAccordingToFilter()
{
    QFETCH(TestingSettings, testingSettings);
    QFETCH(PacsDeviceManager::PacsFilter, filter);
    QFETCH(QList<PacsDevice>, expectedList);

    Settings::setStaticTestingSettings(&testingSettings);

    QList<PacsDevice> list = PacsDeviceManager::getPacsList(filter);

    QCOMPARE(list, expectedList);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::getPacsDeviceById_ShouldReturnExpectedPacsDevice_data()
{
    QTest::addColumn<TestingSettings>("testingSettings");
    QTest::addColumn<QString>("pacsId");
    QTest::addColumn<PacsDevice>("expectedPacsDevice");

    PacsDevice pacs = PACSDeviceTestHelper::createWadoPacsDevice("0", "https://127.0.0.1:9996/wado/");
    TestingSettings testingSettings;
    Settings::SettingsListItemType item;
    item["ID"] = "0";
    item["Type"] = "WADO";
    item["BaseUri"] = pacs.getBaseUri();
    item["Institution"] = pacs.getInstitution();
    item["Location"] = pacs.getLocation();
    item["Description"] = pacs.getDescription();
    testingSettings.addListItem(InputOutputSettings::PacsListConfigurationSectionName, item);

    QTest::newRow("Found") << testingSettings << "0" << pacs;
    QTest::newRow("Not found") << testingSettings << "1" << PacsDevice();
}

void test_PacsDeviceManager::getPacsDeviceById_ShouldReturnExpectedPacsDevice()
{
    QFETCH(TestingSettings, testingSettings);
    QFETCH(QString, pacsId);
    QFETCH(PacsDevice, expectedPacsDevice);

    Settings::setStaticTestingSettings(&testingSettings);

    PacsDevice pacsDevice = PacsDeviceManager::getPacsDeviceById(pacsId);

    QCOMPARE(pacsDevice, expectedPacsDevice);

    Settings::setStaticTestingSettings(nullptr);
}

void test_PacsDeviceManager::removeDuplicatePacsFromList_ShouldReturnListWithoutDuplicatePACS_data()
{
    QTest::addColumn<QList<PacsDevice> >("input");
    QTest::addColumn<QList<PacsDevice> >("result");

    PacsDevice pacsDeviceOne = PACSDeviceTestHelper::createDimsePacsDevice("1", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceTwo = PACSDeviceTestHelper::createDimsePacsDevice("2", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceThree = PACSDeviceTestHelper::createDimsePacsDevice("3", "TESTPACSTHREE", "3.3.3.3", 4006);

    QTest::newRow("Empty input list") << QList<PacsDevice>() << QList<PacsDevice>();
    QTest::newRow("Input list without duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceThree)
            << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceThree);
    QTest::newRow("Input list with duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceTwo << pacsDeviceThree << pacsDeviceTwo << pacsDeviceOne)
            << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceThree);
}

void test_PacsDeviceManager::removeDuplicatePacsFromList_ShouldReturnListWithoutDuplicatePACS()
{
    QFETCH(QList<PacsDevice>, input);
    QFETCH(QList<PacsDevice>, result);

    QList<PacsDevice> pacsDevicesWithoutDuplicates = PacsDeviceManager::removeDuplicatePacsFromList(input);

    QCOMPARE(pacsDevicesWithoutDuplicates.count(), result.count());

    QCOMPARE(pacsDevicesWithoutDuplicates, result);
}

void test_PacsDeviceManager::isAddedSamePacsDeviceInList_ShouldReturnIsSamePacsDeviceIsAddedToList_data()
{
    QTest::addColumn<QList<PacsDevice> >("inputPacsDeviceList");
    QTest::addColumn<PacsDevice>("inputPacsDevice");
    QTest::addColumn<bool>("result");

    PacsDevice pacsDeviceOne = PACSDeviceTestHelper::createDimsePacsDevice("1", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceTwo = PACSDeviceTestHelper::createDimsePacsDevice("2", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceThree = PACSDeviceTestHelper::createDimsePacsDevice("3", "TESTPACSTHREE", "3.3.3.3", 4006);
    PacsDevice pacsDeviceFour = PACSDeviceTestHelper::createDimsePacsDevice("4", "TESTPACSFOUR", "4.4.4.4", 4006);

    QTest::newRow("Empty input list") << QList<PacsDevice>() << pacsDeviceOne << false;
    QTest::newRow("Input list without duplicate PACS") << (QList<PacsDevice>() <<  pacsDeviceThree << pacsDeviceFour)
            << pacsDeviceOne << false;
    QTest::newRow("Input list without duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne <<  pacsDeviceThree)
            << pacsDeviceTwo << true;
    QTest::newRow("Input list with duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceThree << pacsDeviceFour)
            << pacsDeviceTwo << true;
}

void test_PacsDeviceManager::isAddedSamePacsDeviceInList_ShouldReturnIsSamePacsDeviceIsAddedToList()
{
    QFETCH(QList<PacsDevice>, inputPacsDeviceList);
    QFETCH(PacsDevice, inputPacsDevice);
    QFETCH(bool, result);

    QCOMPARE(PacsDeviceManager::isAddedSamePacsDeviceInList(inputPacsDeviceList, inputPacsDevice), result);
}

void test_PacsDeviceManager::cleanupTestCase()
{
    Settings::setStaticTestingSettings(nullptr);
}

DECLARE_TEST(test_PacsDeviceManager)

#include "test_pacsdevicemanager.moc"

