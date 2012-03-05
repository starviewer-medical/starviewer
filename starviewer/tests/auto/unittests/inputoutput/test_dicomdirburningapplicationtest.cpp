#include "autotest.h"
#include "dicomdirburningapplicationtest.h"
#include "diagnosistestresult.h"

#include <QString>

using namespace udg;

class TestingDICOMDIRBurningApplicationTest : public DICOMDIRBurningApplicationTest {
public:
    bool m_defined;
    bool m_installed;

protected:
    virtual bool burningApplicationIsDefined(const QString &burningApplication)
    {
        Q_UNUSED(burningApplication);
        return m_defined;
    }

    virtual bool burningApplicationIsInstalled(const QString &file)
    {
        Q_UNUSED(file);
        return m_installed;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResult)

class test_DICOMDIRBurningApplicationTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfDICOMDIRBurningApplicationIsConfigured_data();
    void run_ShouldTestIfDICOMDIRBurningApplicationIsConfigured();
};

void test_DICOMDIRBurningApplicationTest::run_ShouldTestIfDICOMDIRBurningApplicationIsConfigured_data()
{
    /// Entrada
    QTest::addColumn<bool>("testingDefined");
    QTest::addColumn<bool>("testingInstalled");

    /// Sortida
    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    bool unusedBool = false;

    DiagnosisTestResult noConfiguredResult;
    DiagnosisTestProblem noConfiguredProblem(DiagnosisTestProblem::Warning, "There is no DICOMDIR burning application configured", "Install a DICOMDIR burning application and configure Starviewer to use it on Tools > Configuration > DICOMDIR");
    noConfiguredResult.addWarning(noConfiguredProblem);

    DiagnosisTestResult noExistsResult;
    DiagnosisTestProblem noExistsProblem(DiagnosisTestProblem::Error, "DICOMDIR burning application is not installed on the given location", "Provide the correct location on Tools > Configuration > DICOMDIR");
    noExistsResult.addError(noExistsProblem);

    QTest::newRow("burning application is installed") << true << true << DiagnosisTestResult();
    QTest::newRow("burning application is not configured") << false << unusedBool << noConfiguredResult;
    QTest::newRow("burning application is configured but does not exist") << true << false << noExistsResult;
}

void test_DICOMDIRBurningApplicationTest::run_ShouldTestIfDICOMDIRBurningApplicationIsConfigured()
{
    QFETCH(bool, testingDefined);
    QFETCH(bool, testingInstalled);
 
    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    TestingDICOMDIRBurningApplicationTest burningApplicationTest;
    burningApplicationTest.m_defined = testingDefined;
    burningApplicationTest.m_installed = testingInstalled;

    DiagnosisTestResult result = burningApplicationTest.run();
    
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

DECLARE_TEST(test_DICOMDIRBurningApplicationTest)

#include "test_dicomdirburningapplicationtest.moc"
