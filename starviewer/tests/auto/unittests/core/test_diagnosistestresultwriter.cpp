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

protected:
    QFile* createFile()
    {
        // Fem que no ho guardi a cap fitxer
        return new QFile();
    }
    QTextStream* createTextStream(QFile *file)
    {
        // Enlloc d'escriure un fitxer, ho escrivim a un QString
        Q_UNUSED(file);
        return new QTextStream(m_testingFile);
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
    descriptions << "Diagnosis test 1" << "Diagnosis test 2" << "Invalid Test";
    
    DiagnosisTestResultList testsResults;
    testsResults << DiagnosisTestResult(DiagnosisTestResult::Ok, "", "");
    testsResults << DiagnosisTestResult(DiagnosisTestResult::Error, "Error during the course of the test", "No solution available");
    testsResults << DiagnosisTestResult(DiagnosisTestResult::Invalid, "Invalid test", "No solution available");

    
    QString result = QString("Diagnosis test: %1\nResult: Ok\nDescription: \nSolution: \n\n").arg(descriptions.at(0));
    result += QString("Diagnosis test: %1\nResult: Error\nDescription: Error during the course of the test\nSolution: No solution available\n\n").arg(descriptions.at(1));
    result += QString("Diagnosis test: %1\nResult: Invalid\nDescription: Invalid test\nSolution: No solution available\n\n").arg(descriptions.at(2));
    
    QTest::newRow("ok") << descriptions << testsResults << result;
}

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice()
{
    QFETCH(StringList, diagnosisTestDescriptions);
    QFETCH(DiagnosisTestResultList, diagnosisTestResults);
    QFETCH(QString, result);
    
    TestingDiagnosisTestResultWriter writer;
    // Utilitzem una llista per guardar les referències dels DiagnosisTests que creem, per poder-los eliminar.
    QList<TestingDiagnosisTest*> tests;
    for (int i = 0; i < diagnosisTestDescriptions.count(); i++)
    {
        TestingDiagnosisTest *test = new TestingDiagnosisTest();
        test->m_description = diagnosisTestDescriptions.at(i);
        writer.addDiagnosisTest(test, diagnosisTestResults.at(i));

        // Guardem la referència a la llist
        tests << test;
    }
    writer.write();

    QString writerResult = *writer.m_testingFile;
    QCOMPARE(writerResult, result);

    delete writer.m_testingFile;
    // Eliminem els DiagnosisTests que hem creat.
    for (int i = 0; i < tests.count(); i++)
    {
        delete tests[i];
    }
}

DECLARE_TEST(test_DiagnosisTestResultWriter)

#include "test_diagnosistestresultwriter.moc"

