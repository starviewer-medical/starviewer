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

DECLARE_TEST(test_PacsDevice)

#include "test_pacsdevice.moc"
