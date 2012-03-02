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

Q_DECLARE_METATYPE(DiagnosisTestResult)

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
    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    QString unusedString = "";
    bool unusedBool = true;

    DiagnosisTestResult updateAvailableResult;
    updateAvailableResult.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "There is a new version available.", "Contact technical service to request the software update."));

    QString errorString = "Online checker error http://bla.com?a=10-devel&amp;b=prova link";
    DiagnosisTestResult errorResult;
    errorResult.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Online checker error  link", ""));
    
    QTest::newRow("up to date") << true << false << unusedString << DiagnosisTestResult();
    QTest::newRow("update available") << true << true << unusedString << updateAvailableResult;
    QTest::newRow("error") << false << unusedBool << errorString << errorResult;
}

void test_ApplicationVersionTest::run_ShouldTestIfApplicationIsUpToDate()
{
    QFETCH(bool, testingOnlineCheckOk);
    QFETCH(bool, testingNewVersionAvailable);
    QFETCH(QString, testingOlineCheckerErrorDescription);

    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    TestingApplicationVersionTest applicationVersionTest;
    applicationVersionTest.m_testingOnlineCheckOk = testingOnlineCheckOk;
    applicationVersionTest.m_testingNewVersionAvailable = testingNewVersionAvailable;
    applicationVersionTest.m_testingOlineCheckerErrorDescription = testingOlineCheckerErrorDescription;

    DiagnosisTestResult result = applicationVersionTest.run();
    
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

DECLARE_TEST(test_ApplicationVersionTest)

#include "test_applicationversiontest.moc"

