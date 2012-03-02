#include "autotest.h"
#include "diagnosisTest.h"
#include "diagnosistestresult.h"
#include "diagnosistestresultwriter.h"

#include <QObject>
#include <QList>
#include <QFile>
#include <QString>
#include <QTextStream>

using namespace udg;

typedef QList<DiagnosisTestResult> DiagnosisTestResultList;
typedef QList<QString> StringList;

class TestingDiagnosisTestResultWriter : public DiagnosisTestResultWriter {
public:
    QString *m_testingFile;

public:
    TestingDiagnosisTestResultWriter()
        : DiagnosisTestResultWriter()
    {
        m_testingFile = new QString();
    }
    ~TestingDiagnosisTestResultWriter()
    {
        delete m_testingFile;
    }

protected:
    QFile* createFile(const QString &pathFile)
    {
        Q_UNUSED(pathFile);
        // Fem que no ho guardi a cap fitxer
        return new QFile();
    }
    QTextStream* createTextStream(QFile *file)
    {
        // Enlloc d'escriure un fitxer, ho escrivim a un QString
        Q_UNUSED(file);
        return new QTextStream(m_testingFile);
    }

    void writeHead(QXmlStreamWriter &writer)
    {
        writer.writeEmptyElement("head");
    }
};

class TestingDiagnosisTest : public DiagnosisTest {
public:
    QString m_description;

public:
    TestingDiagnosisTest(QObject *parent = 0)
        : DiagnosisTest(parent)
    {
    }

    ~TestingDiagnosisTest()
    {
    }

    DiagnosisTestResult run()
    {
        return DiagnosisTestResult();
    }

    QString getDescription()
    {
        return m_description;
    }
};

Q_DECLARE_METATYPE(DiagnosisTestResultList)
Q_DECLARE_METATYPE(StringList)

class test_DiagnosisTestResultWriter : public QObject {
Q_OBJECT

private slots:
    void write_ShouldWriteTestResultsToAnIODevice_data();
    void write_ShouldWriteTestResultsToAnIODevice();
};

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice_data()
{
    QTest::addColumn<StringList>("diagnosisTestDescriptions");
    QTest::addColumn<DiagnosisTestResultList>("diagnosisTestResults");
    QTest::addColumn<QString>("result");

    StringList descriptions;
    descriptions << "Diagnosis test: OK" << "Diagnosis test: 1 Warning" << "Diagnosis test: 1 Error" << "Diagnosis test: 1 Error & 1 Warning";
    
    DiagnosisTestResultList testsResults;
    //Succeeded DiagnosisTestResult
    testsResults << DiagnosisTestResult();

    // DiagnosisTestResult with one warning
    DiagnosisTestResult warningResult;
    warningResult.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Warning during the course of the test", "Try to solve the warning"));
    testsResults << warningResult;

    // DiagnosisTestResult with one error
    DiagnosisTestResult errorResult;
    errorResult.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Error during the course of the test", "No solution available"));
    testsResults << errorResult;

    // DiagnosisTestResult with one error and one warning
    DiagnosisTestResult errorAndWarningResult;
    errorAndWarningResult.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Warning during the course of the test", "Try to solve the warning"));
    errorAndWarningResult.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Error during the course of the test", "No solution available"));
    testsResults << errorAndWarningResult;

    QString result = "<?xml version=\"1.0\"?><!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"><html><head/><body><div class=\"buttons\"><span id=\"succeededButton\" class=\"button\"><span class=\"buttonDescription\">1 succeeded</span></span><span id=\"errorButton\" class=\"button\"><span class=\"buttonDescription\">2 errors</span></span><span id=\"warningButton\" class=\"button\"><span class=\"buttonDescription\">1 warnings</span></span></div><div class=\"tests\"><div class=\"succeededTest\"><div class=\"result\"><div class=\"description\">Diagnosis test: OK</div></div></div><div class=\"warningTest\"><div class=\"result\"><div class=\"description\">Diagnosis test: 1 Warning</div></div><div class=\"problems\"><ul class=\"error\"/><ul class=\"warning\"><li><strong>Warning during the course of the test</strong><br/>Try to solve the warning</li></ul></div></div><div class=\"errorTest\"><div class=\"result\"><div class=\"description\">Diagnosis test: 1 Error</div></div><div class=\"problems\"><ul class=\"error\"><li><strong>Error during the course of the test</strong><br/>No solution available</li></ul><ul class=\"warning\"/></div></div><div class=\"errorTest\"><div class=\"result\"><div class=\"description\">Diagnosis test: 1 Error &amp; 1 Warning</div></div><div class=\"problems\"><ul class=\"error\"><li><strong>Error during the course of the test</strong><br/>No solution available</li></ul><ul class=\"warning\"><li><strong>Warning during the course of the test</strong><br/>Try to solve the warning</li></ul></div></div></div></body></html>";
    
    QTest::newRow("ok") << descriptions << testsResults << result;
}

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice()
{
    QFETCH(StringList, diagnosisTestDescriptions);
    QFETCH(DiagnosisTestResultList, diagnosisTestResults);
    QFETCH(QString, result);
    
    TestingDiagnosisTestResultWriter writer;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > diagnosisTestsToWrite;

    for (int i = 0; i < diagnosisTestDescriptions.count(); i++)
    {
        TestingDiagnosisTest *test = new TestingDiagnosisTest();
        test->m_description = diagnosisTestDescriptions.at(i);
        diagnosisTestsToWrite << QPair<DiagnosisTest*, DiagnosisTestResult>(test, diagnosisTestResults.at(i));
    }

    writer.setDiagnosisTests(diagnosisTestsToWrite);
    writer.write("");

    QRegExp enters("\\s*\n\\s*");
    QString writerResult = writer.m_testingFile->remove(enters);
    QCOMPARE(writerResult, result);

    // Eliminem els DiagnosisTests que hem creat.
    for (int i = 0; i < diagnosisTestsToWrite.count(); i++)
    {
        delete diagnosisTestsToWrite[i].first;
    }
}

DECLARE_TEST(test_DiagnosisTestResultWriter)

#include "test_diagnosistestresultwriter.moc"

