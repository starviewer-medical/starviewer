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

    TestingSettings settings0, settings1, settings2, settings3, settings4;

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
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle("^_^");
        pacsDevice.setAddress("0.0.0.0");
        pacsDevice.setQueryRetrieveServiceEnabled(false);
        pacsDevice.setQueryRetrieveServicePort(0);
        pacsDevice.setStoreServiceEnabled(true);
        pacsDevice.setStoreServicePort(1312);
        pacsDevice.setLocation("L2");

        Settings::SettingsListItemType item;
        item["ID"] = "2";
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

    QTest::newRow("Same DIMSE Q/R different Store") << testingSettings << PACSDeviceTestHelper::createPACSDevice("2", "||*||", "cat", 1714);

    QTest::newRow("Same WADO") << testingSettings << PACSDeviceTestHelper::createWadoPacsDevice("3", "https://127.0.0.1:9996/wado/");
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
        item["Type"] = "DIMSE";
        item["AETitle"] = "@.@";
        item["PacsHostname"] = "10.6.222.5";
        item["QueryRetrieveServiceEnabled"] = true;
        item["PacsPort"] = 2446;
        item["StoreServiceEnabled"] = true;
        item["StoreServicePort"] = 2447;
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
    PacsDevice pacs0 = PACSDeviceTestHelper::createPACSDevice("0", "pacs0", "1.0.0.0", 2000);
    PacsDeviceManager::addPacs(pacs0);

    // DIMSE Q/R Default
    PacsDevice pacs1 = PACSDeviceTestHelper::createPACSDevice("1", "pacs1", "1.0.0.1", 2001);
    pacs1.setDefault(true);
    PacsDeviceManager::addPacs(pacs1);

    // DIMSE Q/R Store No default
    PacsDevice pacs2 = PACSDeviceTestHelper::createPACSDevice("2", "pacs2", "1.0.0.2", 2002);
    pacs2.setStoreServiceEnabled(true);
    pacs2.setStoreServicePort(2012);
    PacsDeviceManager::addPacs(pacs2);

    // DIMSE Q/R Store Default
    PacsDevice pacs3 = PACSDeviceTestHelper::createPACSDevice("3", "pacs3", "1.0.0.3", 2003);
    pacs3.setStoreServiceEnabled(true);
    pacs3.setStoreServicePort(2013);
    pacs3.setDefault(true);
    PacsDeviceManager::addPacs(pacs3);

    // DIMSE Store No default
    PacsDevice pacs4 = PACSDeviceTestHelper::createPACSDevice("4", "pacs4", "1.0.0.4", 2004);
    pacs4.setQueryRetrieveServiceEnabled(false);
    pacs4.setStoreServiceEnabled(true);
    pacs4.setStoreServicePort(2014);
    PacsDeviceManager::addPacs(pacs4);

    // DIMSE Store Default
    PacsDevice pacs5 = PACSDeviceTestHelper::createPACSDevice("5", "pacs5", "1.0.0.5", 2005);
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

    Settings::setStaticTestingSettings(nullptr);

    using PDM = PacsDeviceManager;
    using PacsFilter = PDM::PacsFilter;
    using List = QList<PacsDevice>;

    QTest::newRow("Null filter") << settings << PacsFilter(0) << List{};

    QTest::newRow("Q/R") << settings << PacsFilter(PDM::DimseWithQueryRetrieveService) << List{pacs0, pacs1, pacs2, pacs3};
    QTest::newRow("Store") << settings << PacsFilter(PDM::DimseWithStoreService) << List{pacs2, pacs3, pacs4, pacs5};
    QTest::newRow("DIMSE (Q/R | Store)") << settings << PacsFilter(PDM::Dimse) << List{pacs0, pacs1, pacs2, pacs3, pacs4, pacs5};
    QTest::newRow("WADO") << settings << PacsFilter(PDM::Wado) << List{pacs6, pacs7};
    QTest::newRow("All types (DIMSE | WADO)") << settings << PacsFilter(PDM::AllTypes) << List{pacs0, pacs1, pacs2, pacs3, pacs4, pacs5, pacs6, pacs7};
    QTest::newRow("Only default") << settings << PacsFilter(PDM::OnlyDefault) << List{};

    QTest::newRow("Q/R only default") << settings << PacsFilter(PDM::DimseWithQueryRetrieveService | PDM::OnlyDefault) << List{pacs1, pacs3};
    QTest::newRow("Store only default") << settings << PacsFilter(PDM::DimseWithStoreService | PDM::OnlyDefault) << List{pacs3, pacs5};
    QTest::newRow("DIMSE only default") << settings << PacsFilter(PDM::Dimse | PDM::OnlyDefault) << List{pacs1, pacs3, pacs5};
    QTest::newRow("WADO only default") << settings << PacsFilter(PDM::Wado | PDM::OnlyDefault) << List{pacs7};
    QTest::newRow("All types only default") << settings << PacsFilter(PDM::AllTypes | PDM::OnlyDefault) << List{pacs1, pacs3, pacs5, pacs7};
    QTest::newRow("Q/R | WADO") << settings << PacsFilter(PDM::DimseWithQueryRetrieveService | PDM::Wado) << List{pacs0, pacs1, pacs2, pacs3, pacs6, pacs7};
    QTest::newRow("Store | WADO") << settings << PacsFilter(PDM::DimseWithStoreService | PDM::Wado) << List{pacs2, pacs3, pacs4, pacs5, pacs6, pacs7};

    QTest::newRow("Q/R | WADO only default") << settings << PacsFilter(PDM::DimseWithQueryRetrieveService | PDM::Wado | PDM::OnlyDefault)
                                             << List{pacs1, pacs3, pacs7};
    QTest::newRow("Store | WADO only default") << settings << PacsFilter(PDM::DimseWithStoreService | PDM::Wado | PDM::OnlyDefault)
                                               << List{pacs3, pacs5, pacs7};
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

    PacsDevice pacsDeviceOne = PACSDeviceTestHelper::createPACSDevice("1", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceTwo = PACSDeviceTestHelper::createPACSDevice("2", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceThree = PACSDeviceTestHelper::createPACSDevice("3", "TESTPACSTHREE", "3.3.3.3", 4006);

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

    PacsDevice pacsDeviceOne = PACSDeviceTestHelper::createPACSDevice("1", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceTwo = PACSDeviceTestHelper::createPACSDevice("2", "TESTPACSONE", "1.1.1.1", 4006);
    PacsDevice pacsDeviceThree = PACSDeviceTestHelper::createPACSDevice("3", "TESTPACSTHREE", "3.3.3.3", 4006);
    PacsDevice pacsDeviceFour = PACSDeviceTestHelper::createPACSDevice("4", "TESTPACSFOUR", "4.4.4.4", 4006);

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

