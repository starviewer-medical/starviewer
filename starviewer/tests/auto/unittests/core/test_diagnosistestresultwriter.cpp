/*@
    "name": "test_DiagnosisTestResultWriter",
    "requirements": ["other_features.diagnosis_tests"]
 */

#include "autotest.h"
#include "diagnosistestresultwriter.h"

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "starviewerapplication.h"

#include <QFile>
#include <QObject>
#include <QStringList>
#include <QTextStream>

using namespace udg;

typedef QList<DiagnosisTestResult> DiagnosisTestResultList;

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

    void writeSystemInformation(QXmlStreamWriter &writer)
    {
        Q_UNUSED(writer);
    }
};

class TestingDiagnosisTest : public DiagnosisTest {
public:
    QString m_description;

public:
    TestingDiagnosisTest(QObject *parent = nullptr)
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

class test_DiagnosisTestResultWriter : public QObject {
Q_OBJECT

private slots:
    void write_ShouldWriteTestResultsToAnIODevice_data();
    void write_ShouldWriteTestResultsToAnIODevice();
};

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice_data()
{
    QTest::addColumn<QStringList>("diagnosisTestDescriptions");
    QTest::addColumn<DiagnosisTestResultList>("diagnosisTestResults");
    QTest::addColumn<QString>("result");

    QStringList descriptions;
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

    QString result(R"(<?xml version="1.0"?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html><head/>)"
                   R"(<body><div class="buttons"><span id="succeededButton" class="button"><span class="buttonDescription">1 succeeded</span></span>)"
                   R"(<span id="errorButton" class="button"><span class="buttonDescription">2 errors</span></span><span id="warningButton" class="button">)"
                   R"(<span class="buttonDescription">1 warnings</span></span></div><div class="tests"><div class="information"><div class="result">)"
                   R"(<div class="description">Test details</div></div><div class="info"><ul><li>Starviewer %1</li><li>Timestamp: 2017-10-01T17:14:00</li>)"
                   R"(</ul></div></div><div class="succeededTest"><div class="result"><div class="description">Diagnosis test: OK</div></div></div>)"
                   R"(<div class="warningTest"><div class="result"><div class="description">Diagnosis test: 1 Warning</div></div><div class="problems">)"
                   R"(<ul class="error"/><ul class="warning"><li><strong>Warning during the course of the test</strong><br/>Try to solve the warning</li></ul>)"
                   R"(</div></div><div class="errorTest"><div class="result"><div class="description">Diagnosis test: 1 Error</div></div>)"
                   R"(<div class="problems"><ul class="error"><li><strong>Error during the course of the test</strong><br/>No solution available</li></ul>)"
                   R"(<ul class="warning"/></div></div><div class="errorTest"><div class="result">)"
                   R"(<div class="description">Diagnosis test: 1 Error &amp; 1 Warning</div></div><div class="problems"><ul class="error"><li>)"
                   R"(<strong>Error during the course of the test</strong><br/>No solution available</li></ul><ul class="warning"><li>)"
                   R"(<strong>Warning during the course of the test</strong><br/>Try to solve the warning</li></ul></div></div></div></body></html>)");
    result = result.arg(StarviewerVersionString);
    
    QTest::newRow("ok") << descriptions << testsResults << result;
}

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice()
{
    QFETCH(QStringList, diagnosisTestDescriptions);
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

    writer.setDiagnosisTests(std::move(diagnosisTestsToWrite), std::move(QDateTime(QDate(2017, 10, 1), QTime(17, 14))));
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

