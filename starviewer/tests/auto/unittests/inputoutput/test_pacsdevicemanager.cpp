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
};

Q_DECLARE_METATYPE(QList<PacsDevice>)

void test_PacsDeviceManager::removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS_data()
{
    QTest::addColumn<QList<PacsDevice> >("input");
    QTest::addColumn<QList<PacsDevice> >("result");

    PacsDevice pacsDeviceOne = PACSDeviceTestHelper::createPACSDeviceByID("1");
    PacsDevice pacsDeviceTwo = PACSDeviceTestHelper::createPACSDeviceByID("2");
    PacsDevice pacsDeviceThree = PACSDeviceTestHelper::createPACSDeviceByID("3");

    QTest::newRow("Empty input list") << QList<PacsDevice>() << QList<PacsDevice>();
    QTest::newRow("Input list without duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceTwo << pacsDeviceThree)
            << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceTwo << pacsDeviceThree);
    QTest::newRow("Input list with duplicate PACS") << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceTwo << pacsDeviceThree << pacsDeviceTwo << pacsDeviceOne)
            << (QList<PacsDevice>() << pacsDeviceOne << pacsDeviceTwo << pacsDeviceThree);
}

void test_PacsDeviceManager::removeDuplicatePACS_ShouldReturnListWithoutDuplicatePACS()
{
    QFETCH(QList<PacsDevice>, input);
    QFETCH(QList<PacsDevice>, result);

    QList<PacsDevice> pacsDevicesWithoutDuplicates = PacsDeviceManager::removeDuplicatePACS(input);

    QCOMPARE(pacsDevicesWithoutDuplicates.count(), result.count());

    QCOMPARE(pacsDevicesWithoutDuplicates, result);
}

DECLARE_TEST(test_PacsDeviceManager)

#include "test_pacsdevicemanager.moc"

