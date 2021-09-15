/*@
    "name": "test_MachineInformation",
    "requirements": ["other_features.diagnosis_tests"]
 */

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
    void getDomain_ShouldReturnUserDomainInWindows_data();
    void getDomain_ShouldReturnUserDomainInWindows();
};

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
