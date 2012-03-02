#include "autotest.h"
#include "firewallaccess.h"
#include "firewallaccesstest.h"
#include "diagnosistestresult.h"
#include "starviewerapplication.h"

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
Q_DECLARE_METATYPE(DiagnosisTestResult)

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

    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    DiagnosisTestResult firewallBlockingWarning;
    firewallBlockingWarning.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Firewall is blocking", QString("Add %1 to the list of applications that have access through firewall on Control Panel > Firewall").arg(ApplicationNameString)));
    DiagnosisTestResult firewallError;
    firewallError.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Firewall error", "Contact technical service to evaluate the problem"));

    QTest::newRow("access") << FirewallAccess::FirewallIsAccessible << ""
                            << DiagnosisTestResult();
    
    QTest::newRow("blocked") << FirewallAccess::FirewallIsBlocking << "Firewall is blocking"
                             << firewallBlockingWarning;

    QTest::newRow("error") << FirewallAccess::FirewallError << "Firewall error"
                           << firewallError;
}

void test_FirewallAccessTest::run_ShouldTestIfStarviewerHaveAccessThroughFirewall()
{
    QFETCH(FirewallAccess::FirewallStatus, testingStatus);
    QFETCH(QString, testingErrorString);

    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    TestingFirewallAccessTest firewallAccessTest;
    firewallAccessTest.m_testingStatus = testingStatus;
    firewallAccessTest.m_testingErrorString = testingErrorString;

    DiagnosisTestResult result = firewallAccessTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResult.getState());
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getErrors().size(), testingDiagnosisTestResult.getErrors().size());
        QCOMPARE(result.getWarnings().size(), testingDiagnosisTestResult.getWarnings().size());

        QListIterator<DiagnosisTestProblem> resultIterator(result.getErrors() + result.getWarnings());
        QListIterator<DiagnosisTestProblem> testingResultIterator(testingDiagnosisTestResult.getErrors() + testingDiagnosisTestResult.getWarnings());
        while (resultIterator.hasNext() && testingResultIterator.hasNext())
        {
            DiagnosisTestProblem problem = resultIterator.next();
            DiagnosisTestProblem testingProblem = testingResultIterator.next();

            QCOMPARE(problem.getDescription(), testingProblem.getDescription());
            QCOMPARE(problem.getSolution(), testingProblem.getSolution());
        }
    }
}

DECLARE_TEST(test_FirewallAccessTest)

#include "test_firewallaccesstest.moc"

