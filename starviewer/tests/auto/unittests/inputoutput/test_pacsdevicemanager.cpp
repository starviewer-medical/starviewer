#include "autotest.h"

#include "pacsdevicemanager.h"
#include "pacsdevice.h"
#include "pacsdevicetesthelper.h"

using namespace udg;
using namespace testing;

class test_PacsDeviceManager : public QObject {
Q_OBJECT

private slots:
    void removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS_data();
    void removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS();

    void isAddedSamePacsDeviceInList_ShouldRetunIsSamePacsDeviceIsAddedToList_data();
    void isAddedSamePacsDeviceInList_ShouldRetunIsSamePacsDeviceIsAddedToList();

};

Q_DECLARE_METATYPE(QList<PacsDevice>)
Q_DECLARE_METATYPE(PacsDevice)

void test_PacsDeviceManager::removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS_data()
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

void test_PacsDeviceManager::removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS()
{
    QFETCH(QList<PacsDevice>, input);
    QFETCH(QList<PacsDevice>, result);

    QList<PacsDevice> pacsDevicesWithoutDuplicates = PacsDeviceManager::removeDuplicateSamePACS(input);

    QCOMPARE(pacsDevicesWithoutDuplicates.count(), result.count());

    QCOMPARE(pacsDevicesWithoutDuplicates, result);
}

void test_PacsDeviceManager::isAddedSamePacsDeviceInList_ShouldRetunIsSamePacsDeviceIsAddedToList_data()
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

void test_PacsDeviceManager::isAddedSamePacsDeviceInList_ShouldRetunIsSamePacsDeviceIsAddedToList()
{
    QFETCH(QList<PacsDevice>, inputPacsDeviceList);
    QFETCH(PacsDevice, inputPacsDevice);
    QFETCH(bool, result);

    QCOMPARE(PacsDeviceManager::isAddedSamePacsDeviceInList(inputPacsDeviceList, inputPacsDevice), result);
}

DECLARE_TEST(test_PacsDeviceManager)

#include "test_pacsdevicemanager.moc"

