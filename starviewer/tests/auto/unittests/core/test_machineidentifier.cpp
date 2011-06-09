#include "autotest.h"
#include "machineidentifier.h"

using namespace udg;

class TestingMachineIdentifier : public MachineIdentifier {
public:
    QString m_macAddress;
    QString m_domain;

private:
    virtual QString getMacAddress()
    {
        return m_macAddress;
    }

    virtual QString getDomain()
    {
        return m_domain;
    }
};

class test_MachineIdentifier : public QObject {
Q_OBJECT

private slots:
    void getMachineID_ShouldReturnMachineID_data();
    void getMachineID_ShouldReturnMachineID();

    void getGroupID_ShouldReturnGroupID_data();
    void getGroupID_ShouldReturnGroupID();
};

void test_MachineIdentifier::getMachineID_ShouldReturnMachineID()
{
    QFETCH(QString, string);
    QFETCH(QString, encryptedString);

    TestingMachineIdentifier machineIdentifier;
    machineIdentifier.m_macAddress = string;

    QCOMPARE(machineIdentifier.getMachineID(), encryptedString);
}

void test_MachineIdentifier::getMachineID_ShouldReturnMachineID_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("encryptedString");

    QTest::newRow("mac address") << "58:b0:35:84:79:6e" << "F1mLPyaqrKcBlNthEadh6A450LI";
    QTest::newRow("long mac address") << "e8:06:88:ff:fe:ac:f0:02" << "SPI2NgaylToRwl1fxXztOdjlkko";
    QTest::newRow("empty mac address") << "" << "2jmj7l5rSw0yVb_vlWAYkK_YBwk";
}

void test_MachineIdentifier::getGroupID_ShouldReturnGroupID()
{
    QFETCH(QString, string);
    QFETCH(QString, encryptedString);

    TestingMachineIdentifier machineIdentifier;
    machineIdentifier.m_domain = string;

    QCOMPARE(machineIdentifier.getGroupID(), encryptedString);
}

void test_MachineIdentifier::getGroupID_ShouldReturnGroupID_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("encryptedString");

    QTest::newRow("domain1") << "ICS" << "2sUnc7NHwvNpOhPW_ZNXoFL81PU";
    QTest::newRow("domain2") << "DOMINILAB" << "Fx_-T7KE1UPzPv63qG74JO0Ac-o";
    QTest::newRow("domain3") << "HTRUETA" << "-wA0f9AEbGG2-suASB1IB6-VPXk";
    QTest::newRow("empty domain") << "" << "2jmj7l5rSw0yVb_vlWAYkK_YBwk";
}

DECLARE_TEST(test_MachineIdentifier)

#include "test_machineidentifier.moc"

