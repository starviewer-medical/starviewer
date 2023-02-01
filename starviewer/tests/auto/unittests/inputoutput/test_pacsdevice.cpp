/*@
    "name": "test_PacsDevice",
    "requirements": ["archive.dimse", "archive.wado"]
 */

#include "autotest.h"
#include "pacsdevice.h"

#include "coresettings.h"
#include "pacsdevicetesthelper.h"
#include "testingsettings.h"

using namespace udg;
using namespace testing;

class test_PacsDevice : public QObject {
Q_OBJECT

private slots:
    void initTestCase();

    void setDefault_BehavesAsExpected_data();
    void setDefault_BehavesAsExpected();

    void isSamePacsDevice_ShouldCheckIfIsSamePacs_data();
    void isSamePacsDevice_ShouldCheckIfIsSamePacs();

    void cleanupTestCase();

private:
    std::unique_ptr<TestingSettings> testingSettings;   // pointer because it crashes in constructor if declared in "stack"
};

Q_DECLARE_METATYPE(PacsDevice)

void test_PacsDevice::initTestCase()
{
    testingSettings = std::make_unique<TestingSettings>();
    Settings::setStaticTestingSettings(testingSettings.get());
}

void test_PacsDevice::setDefault_BehavesAsExpected_data()
{
    QTest::addColumn<QString>("settingBefore");
    QTest::addColumn<PacsDevice>("pacsDevice");
    QTest::addColumn<bool>("setDefault");
    QTest::addColumn<QString>("expectedSettingAfter");

    PacsDevice dimse, wado, hybrid;

    dimse.setType(PacsDevice::Type::Dimse);
    dimse.setAETitle("PACS");
    dimse.setAddress("1.2.3.4");
    dimse.setQueryRetrieveServicePort(1234);
    QString dimseDefault = dimse.getKeyName() + PacsDevice::DefaultPacsListSeparator;

    wado.setType(PacsDevice::Type::Wado);
    wado.setBaseUri(QUrl("http://example.com/"));
    QString wadoDefault = wado.getKeyName() + PacsDevice::DefaultPacsListSeparator;

    hybrid.setType(PacsDevice::Type::WadoUriDimse);
    hybrid.setAETitle(dimse.getAETitle());  // using same values as above in all fields to catch possible "aliasing" problems
    hybrid.setAddress(dimse.getAddress());
    hybrid.setQueryRetrieveServicePort(dimse.getQueryRetrieveServicePort());
    hybrid.setBaseUri(wado.getBaseUri());
    QString hybridDefault = hybrid.getKeyName() + PacsDevice::DefaultPacsListSeparator;

    QString empty;

    QTest::newRow("empty, add dimse") << empty << dimse << true << dimseDefault;
    QTest::newRow("empty, add wado") << empty << wado << true << wadoDefault;
    QTest::newRow("empty, add hybrid") << empty << hybrid << true << hybridDefault;

    QTest::newRow("dimse, add dimse") << dimseDefault << dimse << true << dimseDefault;
    QTest::newRow("wado, add wado") << wadoDefault << wado << true << wadoDefault;
    QTest::newRow("hybrid, add hybrid") << hybridDefault << hybrid << true << hybridDefault;

    QTest::newRow("hybrid, add wado") << hybridDefault << wado << true << hybridDefault + wadoDefault;
    QTest::newRow("wado + dimse, add hybrid") << wadoDefault + dimseDefault << hybrid << true << wadoDefault + dimseDefault + hybridDefault;
    QTest::newRow("hybrid + wado + dimse, add wado") << hybridDefault + wadoDefault + dimseDefault << wado << true
                                                     << hybridDefault + wadoDefault + dimseDefault;

    QTest::newRow("empty, remove dimse") << empty << dimse << false << empty;
    QTest::newRow("empty, remove wado") << empty << wado << false << empty;
    QTest::newRow("empty, remove hybrid") << empty << hybrid << false << empty;

    QTest::newRow("dimse, remove dimse") << dimseDefault << dimse << false << empty;
    QTest::newRow("wado, remove wado") << wadoDefault << wado << false << empty;
    QTest::newRow("hybrid, remove hybrid") << hybridDefault << hybrid << false << empty;

    QTest::newRow("wado + dimse + hybrid -> remove hybrid (last)") << wadoDefault + dimseDefault + hybridDefault << hybrid << false
                                                                   << wadoDefault + dimseDefault;
    QTest::newRow("wado + dimse + hybrid -> remove wado (first)") << wadoDefault + dimseDefault + hybridDefault << wado << false
                                                                  << dimseDefault + hybridDefault;
    QTest::newRow("wado + dimse + hybrid -> remove dimse (middle)") << wadoDefault + dimseDefault + hybridDefault << dimse << false
                                                                    << wadoDefault + hybridDefault;
}

void test_PacsDevice::setDefault_BehavesAsExpected()
{
    QFETCH(QString, settingBefore);
    QFETCH(PacsDevice, pacsDevice);
    QFETCH(bool, setDefault);
    QFETCH(QString, expectedSettingAfter);

    testingSettings->setValue(CoreSettings::DefaultPACSListToQuery2, settingBefore);
    pacsDevice.setDefault(setDefault);
    QString settingAfter = testingSettings->getValue(CoreSettings::DefaultPACSListToQuery2).toString();

    QCOMPARE(settingAfter, expectedSettingAfter);
}

void test_PacsDevice::isSamePacsDevice_ShouldCheckIfIsSamePacs_data()
{
    QTest::addColumn<PacsDevice>("inputPacsDeviceA");
    QTest::addColumn<PacsDevice>("inputPacsDeviceB");
    QTest::addColumn<bool>("result");

    QTest::newRow("DIMSE PACS with same AETitle, address, queryPort and different ID")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("9999", "DCM4CHE", "1.1.1.1", 4006)
            << true;
    QTest::newRow("DIMSE PACS with same AETitle, address, queryPort and ID")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << true;

    QTest::newRow("DIMSE PACS with different AETitle, address, queryPort and same ID")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "2.2.2.2", 1004)
            << false;
    QTest::newRow("DIMSE PACS with different AETitle, address, queryPort and ID")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("9999", "TEST", "2.2.2.2", 1004)
            << false;
    QTest::newRow("DIMSE PACS with same AETitle, address, ID and different queryPort")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 1004)
            << false;
    QTest::newRow("DIMSE PACS with same AETitle, queryPort, ID and different address")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "9.9.9.9", 4006)
            << false;
    QTest::newRow("DIMSE PACS with same address, queryPort, ID and different AETtitle")
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "DCM4CHE", "1.1.1.1", 4006)
            << PACSDeviceTestHelper::createDimsePacsDevice("1", "TEST", "1.1.1.1", 4006)
            << false;

    QTest::newRow("WADO PACS with same ID and base URI")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
            << true;
    QTest::newRow("WADO PACS with same ID and different base URI")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://exemple.cat/wado/")
            << false;
    QTest::newRow("WADO PACS with different ID and same base URI")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
            << PACSDeviceTestHelper::createWadoPacsDevice("2", "https://example.com/wado/")
            << true;
    QTest::newRow("WADO PACS with different ID and base URI")
            << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
            << PACSDeviceTestHelper::createWadoPacsDevice("2", "https://exemple.cat/wado/")
            << false;

    QTest::newRow("Hybrid PACS with same ID, AE title, address, query port and base URI")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << true;
    QTest::newRow("Hybrid PACS with different ID and same AE title, address, query port and base URI")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("2", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << true;
    QTest::newRow("Hybrid PACS with different ID and AE title and same address, query port and base URI")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("2", "....", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << false;
    QTest::newRow("Hybrid PACS with different ID and address and same AE title, query port and base URI")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("2", "asdf", "1.2.3.44", 4444, "https://example.com/wado-uri/")
            << false;
    QTest::newRow("Hybrid PACS with different ID and query port and same AE title, address and base URI")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("2", "asdf", "1.2.3.4", 5555, "https://example.com/wado-uri/")
            << false;
    QTest::newRow("Hybrid PACS with different ID and base URI and same AE title, address and query port")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "asdf", "1.2.3.4", 4444, "https://example.com/wado-uri/")
            << PACSDeviceTestHelper::createWadoUriDimsePacsDevice("2", "asdf", "1.2.3.4", 4444, "https://example.cat/wado-uri/")
            << false;

    PacsDevice hybrid = PACSDeviceTestHelper::createWadoUriDimsePacsDevice("1", "ae", "1.2.3.4", 1234, "https://example.com/");
    PacsDevice dimse(hybrid), wado(hybrid);
    dimse.setType(PacsDevice::Type::Dimse);
    wado.setType(PacsDevice::Type::Wado);
    QTest::newRow("DIMSE vs WADO") << dimse << wado << false;
    QTest::newRow("DIMSE vs hybrid") << dimse << hybrid << false;
    QTest::newRow("WADO vs hybrid") << wado << hybrid << false;
}

void test_PacsDevice::isSamePacsDevice_ShouldCheckIfIsSamePacs()
{
    QFETCH(PacsDevice, inputPacsDeviceA);
    QFETCH(PacsDevice, inputPacsDeviceB);
    QFETCH(bool, result);

    QCOMPARE(inputPacsDeviceA.isSamePacsDevice(inputPacsDeviceB), result);
}

void test_PacsDevice::cleanupTestCase()
{
    Settings::setStaticTestingSettings(nullptr);
}

DECLARE_TEST(test_PacsDevice)

#include "test_pacsdevice.moc"
