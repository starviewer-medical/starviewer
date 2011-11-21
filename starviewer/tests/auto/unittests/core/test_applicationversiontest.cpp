#include "autotest.h"
#include "applicationversiontest.h"
#include "diagnosistestresult.h"

using namespace udg;

class TestingApplicationVersionTest : public ApplicationVersionTest {
public:
    bool m_testingOnlineCheckOk;
    bool m_testingNewVersionAvailable;
    QString m_testingOlineCheckerErrorDescription;

protected:
    virtual void onlineCheck()
    {
        m_onlineCheckOk = m_testingOnlineCheckOk;
        m_newVersionAvailable = m_testingNewVersionAvailable;
        m_olineCheckerErrorDescription = m_testingOlineCheckerErrorDescription;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)

class test_ApplicationVersionTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfApplicationIsUpToDate_data();
    void run_ShouldTestIfApplicationIsUpToDate();
};

void test_ApplicationVersionTest::run_ShouldTestIfApplicationIsUpToDate_data()
{
    // Entrada
    QTest::addColumn<bool>("testingOnlineCheckOk");
    QTest::addColumn<bool>("testingNewVersionAvailable");
    QTest::addColumn<QString>("testingOlineCheckerErrorDescription");
    // Sortida    
    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    QString unusedString = "";
    bool unusedBool = true;

    QTest::newRow("up to date") << true << false << unusedString
                                << DiagnosisTestResult::Ok << "" << "";
    
    QTest::newRow("update available") << true << true << unusedString
                                      << DiagnosisTestResult::Warning << "There is a new version" << "Update";

    QString errorString = "Online checker error";
    QTest::newRow("error") << false << unusedBool << errorString
                           << DiagnosisTestResult::Error << errorString << "";
}

void test_ApplicationVersionTest::run_ShouldTestIfApplicationIsUpToDate()
{
    QFETCH(bool, testingOnlineCheckOk);
    QFETCH(bool, testingNewVersionAvailable);
    QFETCH(QString, testingOlineCheckerErrorDescription);

    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingApplicationVersionTest applicationVersionTest;
    applicationVersionTest.m_testingOnlineCheckOk = testingOnlineCheckOk;
    applicationVersionTest.m_testingNewVersionAvailable = testingNewVersionAvailable;
    applicationVersionTest.m_testingOlineCheckerErrorDescription = testingOlineCheckerErrorDescription;

    DiagnosisTestResult result = applicationVersionTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_ApplicationVersionTest)

#include "test_applicationversiontest.moc"

