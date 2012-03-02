#include "autotest.h"
#include "portinuse.h"
#include "portinusetest.h"
#include "diagnosistestresult.h"
#include "testingportinuse.h"
#include "testingportinusebyanotherapplication.h"

using namespace udg;
using namespace testing;

class TestingPortInUseTest : public PortInUseTest {
public:
    PortInUse::PortInUseStatus m_testingStatus;
    QString m_testingErrorString;
    bool m_testingInUseByAnotherApplication;

    TestingPortInUseTest(int port)
    {
        m_port = port;
    }

protected:
    virtual PortInUse* createPortInUse()
    {
        TestingPortInUse *portInUse = new TestingPortInUse();
        // El port in use by another application s'esborra dins el mètode getOwner de la classe port in use,
        // el qual només es crida si l'status és diferent de PortIsAvailable
        if (m_testingStatus != PortInUse::PortIsAvailable)
        {
            TestingPortInUseByAnotherApplication *portInUseByAnotherApplication = new TestingPortInUseByAnotherApplication();
            portInUseByAnotherApplication->m_testingInUseByAnotherApplication = m_testingInUseByAnotherApplication;
            portInUse->m_testingPortInUseByAnotherApplication = portInUseByAnotherApplication;
        }
        return portInUse;
    }

    virtual void checkIfPortIsInUse(PortInUse *portInUse)
    {
        /// S'ha de fer el cast, ja que els mètodes de setStatus i SetErrorString només els té TestingPortInUse.
        /// Podem fer el cast per que sabem que és un TestingPortInUse ja que l'hem creat al mètode anterior.
        ((TestingPortInUse*)portInUse)->setStatus(m_testingStatus);
        ((TestingPortInUse*)portInUse)->setErrorString(m_testingErrorString);
    }
};

Q_DECLARE_METATYPE(PortInUse::PortInUseStatus)
Q_DECLARE_METATYPE(DiagnosisTestResult)

class test_PortInUseTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfRISPortIsInUse_data();
    void run_ShouldTestIfRISPortIsInUse();
};

void test_PortInUseTest::run_ShouldTestIfRISPortIsInUse_data()
{
    QTest::addColumn<PortInUse::PortInUseStatus>("testingStatus");
    QTest::addColumn<QString>("testingErrorString");
    QTest::addColumn<bool>("testingInUseByAnotherApplication");

    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    QString unusedString = "";
    DiagnosisTestResult portUsedError;
    portUsedError.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Port 0 is already in use by other application", "Try another port or shutdown the application using this port."));

    DiagnosisTestResult portError;
    portError.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Unable to test if port 0 is in use due to error: NetworkError", "Contact technical service to evaluate the problem."));

    QTest::newRow("free port") << PortInUse::PortIsAvailable << unusedString << false
                               << DiagnosisTestResult();
    
    QTest::newRow("port in use by another application") 
                                << PortInUse::PortIsInUse << unusedString << true
                                << portUsedError;

    QTest::newRow("port in use by starviewer") 
                                << PortInUse::PortIsInUse << unusedString << false
                                << DiagnosisTestResult();

    QTest::newRow("port error") << PortInUse::PortCheckError << "NetworkError" << false
                                << portError;
}

void test_PortInUseTest::run_ShouldTestIfRISPortIsInUse()
{
    QFETCH(PortInUse::PortInUseStatus, testingStatus);
    QFETCH(QString, testingErrorString);
    QFETCH(bool, testingInUseByAnotherApplication);

    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    // El port no es fa servir en el test
    TestingPortInUseTest portInUseTest(0);
    portInUseTest.m_testingStatus = testingStatus;
    portInUseTest.m_testingErrorString = testingErrorString;
    portInUseTest.m_testingInUseByAnotherApplication = testingInUseByAnotherApplication;

    DiagnosisTestResult result = portInUseTest.run();
    
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

DECLARE_TEST(test_PortInUseTest)

#include "test_portinusetest.moc"
