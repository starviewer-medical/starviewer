#include "autotest.h"
#include "machineinformation.h"

#include <QNetworkInterface>
#include <QProcessEnvironment>

using namespace udg;

class TestingMachineInformation : public MachineInformation {
public:
    QProcessEnvironment m_systemEnvironment;

private:
    virtual QProcessEnvironment getSystemEnvironment()
    {
        return m_systemEnvironment;
    }
};


class test_MachineInformation : public QObject {
Q_OBJECT

private slots:
    void getMACAddress_CheckIfItsAValidMACAddress();

    void getDomain_ShouldReturnUserDomainInWindows_data();
    void getDomain_ShouldReturnUserDomainInWindows();
};


void test_MachineInformation::getMACAddress_CheckIfItsAValidMACAddress()
{
    QString macAddress = MachineInformation().getMACAddress();
    QVERIFY(!macAddress.isEmpty());

    QRegExp macExpression("^([0-9a-f]{2}([:-]|$)){6}$", Qt::CaseInsensitive);
    QVERIFY2(macExpression.exactMatch(macAddress), qPrintable(QString("Obtained MAC Address: %1").arg(macAddress)));

    //Comprovem que hi hagi alguna interfície amb aquesta adreça MAC
    bool foundInterface = false;
    QNetworkInterface interface;
    QListIterator<QNetworkInterface> iterator(QNetworkInterface::allInterfaces());
    while (!foundInterface && iterator.hasNext())
    {
        interface = iterator.next();
        foundInterface = interface.hardwareAddress() == macAddress;
    }

    QVERIFY(foundInterface);
    QVERIFY(interface.flags().testFlag(QNetworkInterface::IsUp));
    QVERIFY(interface.flags().testFlag(QNetworkInterface::IsRunning));
    QVERIFY(!interface.flags().testFlag(QNetworkInterface::IsLoopBack));
    QVERIFY(!interface.humanReadableName().toLower().contains("bluetooth"));
}

void test_MachineInformation::getDomain_ShouldReturnUserDomainInWindows_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("expectedResult");

#ifdef WIN32
    QTest::newRow("undefined userdomain") << "invalidVariable" << "blablabla" << "";
    QTest::newRow("USERDOMAIN uppercase") << "USERDOMAIN" << "customUserDomain1" << "customUserDomain1";
    QTest::newRow("userdomain lowercase") << "userdomain" << "customUserDomain2" << "customUserDomain2";
    QTest::newRow("userdomain mixing") << "UserDomaiN" << "customUserDomain3" << "customUserDomain3";
#else
    QTest::newRow("undefined userdomain") << "invalidVariable" << "blablabla" << "";
    QTest::newRow("USERDOMAIN uppercase") << "USERDOMAIN" << "customUserDomain1" << "";
    QTest::newRow("userdomain lowercase") << "userdomain" << "customUserDomain2" << "";
    QTest::newRow("userdomain mixing") << "UserDomaiN" << "customUserDomain3" << "";
#endif
}

void test_MachineInformation::getDomain_ShouldReturnUserDomainInWindows()
{
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(QString, expectedResult);

    TestingMachineInformation machineInformation;
    machineInformation.m_systemEnvironment.insert(name, value);

    QCOMPARE(machineInformation.getDomain(), expectedResult);
}

DECLARE_TEST(test_MachineInformation)

#include "test_machineinformation.moc"
