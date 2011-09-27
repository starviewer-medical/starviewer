#include "autotest.h"
#include "portinuse.h"
#include "portinusetest.h"
#include "diagnosistestresult.h"
#include "testingportinuse.h"

using namespace udg;
using namespace testing;

class TestingPortInUseTest : public PortInUseTest {
public:
    PortInUse::PortInUseStatus m_testingStatus;
    QString m_testingErrorString;

protected:
    virtual PortInUse* createPortInUse()
    {
        TestingPortInUse *portInUse = new TestingPortInUse();
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

    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    QString unusedString = "";

    QTest::newRow("free port") << PortInUse::PortIsAvailable << unusedString
                               << DiagnosisTestResult::Ok << unusedString << unusedString;
    
    QTest::newRow("port in use") << PortInUse::PortIsInUse << unusedString
                                 << DiagnosisTestResult::Error << "Port is already in use" << "Try another port or shut down the application using this port";

    QTest::newRow("port error") << PortInUse::PortCheckError << "NetworkError"
                                << DiagnosisTestResult::Error << "Unable to test if port 0 is in use due to error: NetworkError"
                                << unusedString;
}

void test_PortInUseTest::run_ShouldTestIfRISPortIsInUse()
{
    QFETCH(PortInUse::PortInUseStatus, testingStatus);
    QFETCH(QString, testingErrorString);
 
    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingPortInUseTest portInUseTest;
    
    portInUseTest.m_testingStatus = testingStatus;
    portInUseTest.m_testingErrorString = testingErrorString;

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
