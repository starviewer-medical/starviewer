#include "autotest.h"
#include "firewallaccess.h"
#include "firewallaccesstest.h"
#include "diagnosistestresult.h"

using namespace udg;

class TestingFirewallAccess: public FirewallAccess {
public:
    TestingFirewallAccess()
    {
    }

    ~TestingFirewallAccess()
    {
    }

    void setStatus(FirewallAccess::FirewallStatus status)
    {
        m_status = status;
    }

    void setErrorString(QString errorString)
    {
        m_errorString = errorString;
    }
};

class TestingFirewallAccessTest : public FirewallAccessTest {
public:
    FirewallAccess::FirewallStatus m_testingStatus;
    QString m_testingErrorString;

protected:
    virtual FirewallAccess* createFirewall()
    {
        TestingFirewallAccess *firewall = new TestingFirewallAccess();
        return firewall;
    }

    virtual void checkIfStarviewerHaveAccessThroughFirewall(FirewallAccess *firewall)
    {
        /// Podem fer el cast per que sabem que és un testingFirewallAccess ja que l'hem creat al mètode anterior
        ((TestingFirewallAccess*)firewall)->setStatus(m_testingStatus);
        ((TestingFirewallAccess*)firewall)->setErrorString(m_testingErrorString);
    }
};

Q_DECLARE_METATYPE(FirewallAccess::FirewallStatus)
Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)

class test_FirewallAccessTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfStarviewerHaveAccessThroughFirewall_data();
    void run_ShouldTestIfStarviewerHaveAccessThroughFirewall();
};

void test_FirewallAccessTest::run_ShouldTestIfStarviewerHaveAccessThroughFirewall_data()
{
    QTest::addColumn<FirewallAccess::FirewallStatus>("testingStatus");
    QTest::addColumn<QString>("testingErrorString");

    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    QTest::newRow("access") << FirewallAccess::FirewallIsAccessible << ""
                            << DiagnosisTestResult::Ok << "" << "";
    
    QTest::newRow("blocked") << FirewallAccess::FirewallIsBlocking << "Firewall is blocking"
                             << DiagnosisTestResult::Warning << "Firewall is blocking" << "Add Starviewer to the list of allowed applications through firewall";

    QTest::newRow("error") << FirewallAccess::FirewallError << "Firewall error"
                           << DiagnosisTestResult::Error << "Firewall error" << "";
}

void test_FirewallAccessTest::run_ShouldTestIfStarviewerHaveAccessThroughFirewall()
{
    QFETCH(FirewallAccess::FirewallStatus, testingStatus);
    QFETCH(QString, testingErrorString);

    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingFirewallAccessTest firewallAccessTest;
    firewallAccessTest.m_testingStatus = testingStatus;
    firewallAccessTest.m_testingErrorString = testingErrorString;

    DiagnosisTestResult result = firewallAccessTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_FirewallAccessTest)

#include "test_firewallaccesstest.moc"

