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
        TestingPortInUseByAnotherApplication *portInUseByAnotherApplication = new TestingPortInUseByAnotherApplication();
        portInUseByAnotherApplication->m_testingInUseByAnotherApplication = m_testingInUseByAnotherApplication;
        portInUse->m_testingPortInUseByAnotherApplication = portInUseByAnotherApplication;
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
Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)

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

    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    QString unusedString = "";

    QTest::newRow("free port") << PortInUse::PortIsAvailable << unusedString << false
                               << DiagnosisTestResult::Ok << unusedString << unusedString;
    
    QTest::newRow("port in use by another application") 
                                << PortInUse::PortIsInUse << unusedString << true
                                << DiagnosisTestResult::Error << "Port is already in use"
                                << "Try another port or shut down the application using this port";

    QTest::newRow("port in use by starviewer") 
                                << PortInUse::PortIsInUse << unusedString << false
                                << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("port error") << PortInUse::PortCheckError << "NetworkError" << false
                                << DiagnosisTestResult::Error
                                << "Unable to test if port 0 is in use due to error: NetworkError"
                                << unusedString;
}

void test_PortInUseTest::run_ShouldTestIfRISPortIsInUse()
{
    QFETCH(PortInUse::PortInUseStatus, testingStatus);
    QFETCH(QString, testingErrorString);
    QFETCH(bool, testingInUseByAnotherApplication);

    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    // El port no es fa servir en el test
    TestingPortInUseTest portInUseTest(0);
    portInUseTest.m_testingStatus = testingStatus;
    portInUseTest.m_testingErrorString = testingErrorString;
    portInUseTest.m_testingInUseByAnotherApplication = testingInUseByAnotherApplication;

    DiagnosisTestResult result = portInUseTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_PortInUseTest)

#include "test_portinusetest.moc"
