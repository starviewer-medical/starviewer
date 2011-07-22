#include "autotest.h"

#include "dicomsource.h"
#include "pacsdevice.h"
#include "pacsdevicetesthelper.h"

using namespace testing;
using namespace udg;

class test_DICOMSource : public QObject {
Q_OBJECT

private slots:

    void addRetrievePACS_ShouldMergePACSDeviceList_data();
    void addRetrievePACS_ShouldMergePACSDeviceList();

    void addPACSDeviceFromDICOMSource_ShouldMergePACSDevice_data();
    void addPACSDeviceFromDICOMSource_ShouldMergePACSDevice();

    void operatorEqual_data();
    void operatorEqual();

private:

    PacsDevice getPACSIDOne();
    PacsDevice getPACSIDTwo();

    DICOMSource getDICOMSourceWithPACSIDOne();
    DICOMSource getDICOMSourceWithPACSIDTwo();
    DICOMSource getDICOMSourceWithPACSIDOneAndTwo();
};

Q_DECLARE_METATYPE(DICOMSource)
Q_DECLARE_METATYPE(PacsDevice)
Q_DECLARE_METATYPE(QList<PacsDevice>)

void test_DICOMSource::addRetrievePACS_ShouldMergePACSDeviceList_data()
{
    QTest::addColumn<DICOMSource>("inputDICOMSource");
    QTest::addColumn<PacsDevice>("inputPACSDevice");
    QTest::addColumn<QList<PacsDevice> >("result");

    QTest::newRow("Add PACSDevice to Empty DICOMSource") << DICOMSource() << getPACSIDOne() << (QList<PacsDevice>() << getPACSIDOne());
    QTest::newRow("Add PACSDevice to DICOMSource")  << getDICOMSourceWithPACSIDOne() << getPACSIDTwo() << (QList<PacsDevice>() << getPACSIDOne() << getPACSIDTwo());
    QTest::newRow("Add duplicate PACSDevice to DICOMSource") << getDICOMSourceWithPACSIDOne() << getPACSIDOne()  << (QList<PacsDevice>() << getPACSIDOne());
}


void test_DICOMSource::addRetrievePACS_ShouldMergePACSDeviceList()
{
    QFETCH(DICOMSource, inputDICOMSource);
    QFETCH(PacsDevice, inputPACSDevice);
    QFETCH(QList<PacsDevice>, result);

    inputDICOMSource.addRetrievePACS(inputPACSDevice);

    QCOMPARE(inputDICOMSource.getRetrievePACS(), result);
}


void test_DICOMSource::addPACSDeviceFromDICOMSource_ShouldMergePACSDevice_data()
{
    QTest::addColumn<DICOMSource>("inputDICOMSourceA");
    QTest::addColumn<DICOMSource>("inputDICOMSourceB");
    QTest::addColumn< QList<PacsDevice> >("result");

    QTest::newRow("Empty DICOMSource") << getDICOMSourceWithPACSIDOne() << DICOMSource() << (QList<PacsDevice>() << getPACSIDOne());
    QTest::newRow("DICOMSource with duplicate PACSDevice")  << getDICOMSourceWithPACSIDOne() << getDICOMSourceWithPACSIDOne() << (QList<PacsDevice>() << getPACSIDOne());
    QTest::newRow("DICOMSSource with no duplicate PACSDevice") << getDICOMSourceWithPACSIDOne() << getDICOMSourceWithPACSIDTwo() << (QList<PacsDevice>() << getPACSIDOne() << getPACSIDTwo());
}

void test_DICOMSource::addPACSDeviceFromDICOMSource_ShouldMergePACSDevice()
{
    QFETCH(DICOMSource, inputDICOMSourceA);
    QFETCH(DICOMSource, inputDICOMSourceB);
    QFETCH(QList<PacsDevice>, result);

    inputDICOMSourceA.addPACSDeviceFromDICOMSource(inputDICOMSourceB);

    QCOMPARE(inputDICOMSourceA.getRetrievePACS(), result);
}

void test_DICOMSource::operatorEqual_data()
{
    QTest::addColumn<DICOMSource>("DICOMSourceA");
    QTest::addColumn<DICOMSource>("DICOMSourceB");
    QTest::addColumn<bool>("result");

    QTest::newRow("Empty DICOMSource") << DICOMSource() << DICOMSource() << true;
    QTest::newRow("Equal DICOMSource")  << getDICOMSourceWithPACSIDOne() << getDICOMSourceWithPACSIDOne() << true;
    QTest::newRow("Different DICOMSource with same number of PACSDevice") << getDICOMSourceWithPACSIDOne() << getDICOMSourceWithPACSIDTwo() << false;
    QTest::newRow("Different DICOMSource with different number of PACSDevice") << getDICOMSourceWithPACSIDOne() << getDICOMSourceWithPACSIDOneAndTwo() << false;
}

void test_DICOMSource::operatorEqual()
{
    QFETCH(DICOMSource, DICOMSourceA);
    QFETCH(DICOMSource, DICOMSourceB);
    QFETCH(bool, result);

    QCOMPARE(DICOMSourceA == DICOMSourceB, result);
}

PacsDevice test_DICOMSource::getPACSIDOne()
{
    return PACSDeviceTestHelper::createPACSDeviceByID("1");
}

PacsDevice test_DICOMSource::getPACSIDTwo()
{
    return PACSDeviceTestHelper::createPACSDeviceByID("2");
}

DICOMSource test_DICOMSource::getDICOMSourceWithPACSIDOne()
{
    DICOMSource DICOMSourceWithPACSDeviceOne;
    DICOMSourceWithPACSDeviceOne.addRetrievePACS(getPACSIDOne());

    return DICOMSourceWithPACSDeviceOne;
}

DICOMSource test_DICOMSource::getDICOMSourceWithPACSIDTwo()
{
    DICOMSource DICOMSourceWithPACSDeviceTwo;
    DICOMSourceWithPACSDeviceTwo.addRetrievePACS(getPACSIDTwo());

    return DICOMSourceWithPACSDeviceTwo;
}

DICOMSource test_DICOMSource::getDICOMSourceWithPACSIDOneAndTwo()
{
    DICOMSource DICOMSourceWithPACSDeviceOneAndTwo;

    DICOMSourceWithPACSDeviceOneAndTwo.addRetrievePACS(getPACSIDOne());
    DICOMSourceWithPACSDeviceOneAndTwo.addRetrievePACS(getPACSIDTwo());

    return DICOMSourceWithPACSDeviceOneAndTwo;
}

DECLARE_TEST(test_DICOMSource)

#include "test_dicomsource.moc"
