/*@
    "name": "test_PacsDevice",
    "requirements": ["archive.dimse", "archive.wado"]
 */

#include "autotest.h"

#include "pacsdevice.h"
#include "pacsdevicetesthelper.h"

using namespace udg;
using namespace testing;

class test_PacsDevice : public QObject {
Q_OBJECT

private slots:
    void isSamePacsDevice_ShouldCheckIfIsSamePacs_data();
    void isSamePacsDevice_ShouldCheckIfIsSamePacs();
};

Q_DECLARE_METATYPE(PacsDevice)

void test_PacsDevice::isSamePacsDevice_ShouldCheckIfIsSamePacs_data()
{
    QTest::addColumn<PacsDevice>("inputPacsDeviceA");
    QTest::addColumn<PacsDevice>("inputPacsDeviceB");
    QTest::addColumn<bool>("result");

    QTest::newRow("PACS with same AETitle, address, queryPort and different ID") << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("9999", "DCM4CHE", "1.1.1.1", 4006) << true;
    QTest::newRow("PACS with same AETitle, address, queryPort ID") << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006) << true;

    QTest::newRow("PACS with different AETitle, address, queryPort and same ID") << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "2.2.2.2", 1004) << false;
    QTest::newRow("PACS with different AETitle, address, queryPort and ID") << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("9999", "TEST", "2.2.2.2", 1004) << false;
    QTest::newRow("PACS with same AETitle, address, ID and different queryPort")  << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 1004) << false;
    QTest::newRow("PACS with same AETitle, queryPort, ID and different address")  << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "9.9.9.9", 4006) << false;
    QTest::newRow("PACS with same address, queryPort, ID and different AETtitle")  << PACSDeviceTestHelper::createPACSDevice("1", "DCM4CHE", "1.1.1.1", 4006)
        << PACSDeviceTestHelper::createPACSDevice("1", "TEST", "1.1.1.1", 4006) << false;

    QTest::newRow("WADO PACS with same ID and base URI") << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
                                                         << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/") << true;
    QTest::newRow("WADO PACS with same ID and different base URI") << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
                                                                   << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://exemple.cat/wado/") << false;
    QTest::newRow("WADO PACS with different ID and same base URI") << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
                                                                   << PACSDeviceTestHelper::createWadoPacsDevice("2", "https://example.com/wado/") << true;
    QTest::newRow("WADO PACS with different ID and base URI") << PACSDeviceTestHelper::createWadoPacsDevice("1", "https://example.com/wado/")
                                                              << PACSDeviceTestHelper::createWadoPacsDevice("2", "https://exemple.cat/wado/") << false;
}

void test_PacsDevice::isSamePacsDevice_ShouldCheckIfIsSamePacs()
{
    QFETCH(PacsDevice, inputPacsDeviceA);
    QFETCH(PacsDevice, inputPacsDeviceB);
    QFETCH(bool, result);

    QCOMPARE(inputPacsDeviceA.isSamePacsDevice(inputPacsDeviceB), result);
}

DECLARE_TEST(test_PacsDevice)

#include "test_pacsdevice.moc"
