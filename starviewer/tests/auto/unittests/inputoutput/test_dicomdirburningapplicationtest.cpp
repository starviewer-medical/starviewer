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

Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)

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
    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    QString unusedString = "";
    bool unusedBool = false;

    QTest::newRow("burning application is installed") << true << true << DiagnosisTestResult::Ok << unusedString << unusedString;

    QTest::newRow("burning application is not configured") << false << unusedBool
                                                           << DiagnosisTestResult::Warning
                                                           << "There is no DICOMDIR burning application configured"
                                                           << "Install a DICOMDIR burning application";
    
    QTest::newRow("burning application is configured but does not exist") << true << false << DiagnosisTestResult::Error
                                                                          << "DICOMDIR burning application is not installed on settings folder"
                                                                          << "Please reinstall the application or update the configuration";
}

void test_DICOMDIRBurningApplicationTest::run_ShouldTestIfDICOMDIRBurningApplicationIsConfigured()
{
    QFETCH(bool, testingDefined);
    QFETCH(bool, testingInstalled);
 
    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingDICOMDIRBurningApplicationTest burningApplicationTest;
    burningApplicationTest.m_defined = testingDefined;
    burningApplicationTest.m_installed = testingInstalled;

    DiagnosisTestResult result = burningApplicationTest.run();
    
    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
    }
}

DECLARE_TEST(test_DICOMDIRBurningApplicationTest)

#include "test_dicomdirburningapplicationtest.moc"
