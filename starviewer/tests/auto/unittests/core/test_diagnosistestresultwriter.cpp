/*@
    "name": "test_DiagnosisTestResultWriter",
    "requirements": ["other_features.diagnosis_tests"]
 */

#include "autotest.h"
#include "diagnosistestresultwriter.h"

#include "diagnosistest.h"
#include "diagnosistestresult.h"

#include <QBuffer>

using namespace udg;

class TestingDiagnosisTest : public DiagnosisTest
{
public:
    TestingDiagnosisTest(QString description, QObject *parent = nullptr)
        : DiagnosisTest(parent), m_description(std::move(description))
    {
    }

    ~TestingDiagnosisTest() override
    {
    }

    DiagnosisTestResult run() override
    {
        return DiagnosisTestResult();
    }

    QString getDescription() const override
    {
        return m_description;
    }

private:
    QString m_description;

};

Q_DECLARE_METATYPE(DiagnosisTestResultWriter)

class test_DiagnosisTestResultWriter : public QObject {
Q_OBJECT

private slots:
    void write_ShouldWriteTestResultsToAnIODevice_data();
    void write_ShouldWriteTestResultsToAnIODevice();
};

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice_data()
{
    QTest::addColumn<DiagnosisTestResultWriter>("writer");
    QTest::addColumn<QString>("expectedOutput");

    {
        DiagnosisTestResultWriter writer;
        QString expectedOutput("[\n"
                               "]\n"
                               "");
        QTest::newRow("no informations and no tests") << writer << expectedOutput;
    }

    {
        DiagnosisTestResultWriter writer;
        writer.setInformations({QPair<QString, QStringList>("Info 1", {"Whatever"}),
                                QPair<QString, QStringList>("Info 2", {"Lorem ipsum", "dolor sit amet"})});
        QString expectedOutput(R"([)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Info 1",)""\n"
                               R"(        "details": [)""\n"
                               R"(            "Whatever")""\n"
                               R"(        ],)""\n"
                               R"(        "state": "info")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Info 2",)""\n"
                               R"(        "details": [)""\n"
                               R"(            "Lorem ipsum",)""\n"
                               R"(            "dolor sit amet")""\n"
                               R"(        ],)""\n"
                               R"(        "state": "info")""\n"
                               R"(    })""\n"
                               R"(])""\n"
                               R"()");
        QTest::newRow("only informations") << writer << expectedOutput;
    }

    {
        TestingDiagnosisTest *test1 = new TestingDiagnosisTest("Test 1", this);
        DiagnosisTestResult result1;
        result1.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Self-destruct sequence activated", "Run for your life!"));
        TestingDiagnosisTest *test2 = new TestingDiagnosisTest("Test 2", this);
        DiagnosisTestResult result2;
        result2.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "404 Not found", "Go somewhere else"));
        TestingDiagnosisTest *test3 = new TestingDiagnosisTest("Test 3", this);
        DiagnosisTestResult result3;
        DiagnosisTestResultWriter writer;
        writer.setDiagnosisTests({qMakePair(test1, result1),
                                  qMakePair(test2, result2),
                                  qMakePair(test3, result3)});
        QString expectedOutput(R"([)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 1",)""\n"
                               R"(        "problems": [)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Self-destruct sequence activated",)""\n"
                               R"(                "solution": "Run for your life!",)""\n"
                               R"(                "state": "warning")""\n"
                               R"(            })""\n"
                               R"(        ],)""\n"
                               R"(        "state": "warning")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 2",)""\n"
                               R"(        "problems": [)""\n"
                               R"(            {)""\n"
                               R"(                "description": "404 Not found",)""\n"
                               R"(                "solution": "Go somewhere else",)""\n"
                               R"(                "state": "error")""\n"
                               R"(            })""\n"
                               R"(        ],)""\n"
                               R"(        "state": "error")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 3",)""\n"
                               R"(        "state": "correct")""\n"
                               R"(    })""\n"
                               R"(])""\n"
                               R"()");
        QTest::newRow("only test results") << writer << expectedOutput;
    }

    {
        TestingDiagnosisTest *test1 = new TestingDiagnosisTest("Test 1", this);
        DiagnosisTestResult result1;
        TestingDiagnosisTest *test2 = new TestingDiagnosisTest("Test 2", this);
        DiagnosisTestResult result2;
        result2.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Disk almost full", "Delete junk"));
        TestingDiagnosisTest *test3 = new TestingDiagnosisTest("Test 3", this);
        DiagnosisTestResult result3;
        result3.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Biohazard", "Quarantine"));
        result3.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Monarchy", "Guillotine"));
        result3.addError(DiagnosisTestProblem(DiagnosisTestProblem::Error, "Spain", "Republic of Catalonia"));
        result3.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "Danger", "Keep out"));
        DiagnosisTestResultWriter writer;
        writer.setDiagnosisTests({qMakePair(test1, result1),
                                  qMakePair(test2, result2),
                                  qMakePair(test3, result3)});
        writer.setInformations({QPair<QString, QStringList>("Info 1", {"L'all ho és tot pels anglesos", "https://www.wikipedia.org/", "꽲 한글 조선글"})});
        QString expectedOutput(R"([)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Info 1",)""\n"
                               R"(        "details": [)""\n"
                               R"(            "L'all ho és tot pels anglesos",)""\n"
                               R"(            "https://www.wikipedia.org/",)""\n"
                               R"(            "꽲 한글 조선글")""\n"
                               R"(        ],)""\n"
                               R"(        "state": "info")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 1",)""\n"
                               R"(        "state": "correct")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 2",)""\n"
                               R"(        "problems": [)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Disk almost full",)""\n"
                               R"(                "solution": "Delete junk",)""\n"
                               R"(                "state": "warning")""\n"
                               R"(            })""\n"
                               R"(        ],)""\n"
                               R"(        "state": "warning")""\n"
                               R"(    },)""\n"
                               R"(    {)""\n"
                               R"(        "description": "Test 3",)""\n"
                               R"(        "problems": [)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Monarchy",)""\n"
                               R"(                "solution": "Guillotine",)""\n"
                               R"(                "state": "error")""\n"
                               R"(            },)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Spain",)""\n"
                               R"(                "solution": "Republic of Catalonia",)""\n"
                               R"(                "state": "error")""\n"
                               R"(            },)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Biohazard",)""\n"
                               R"(                "solution": "Quarantine",)""\n"
                               R"(                "state": "warning")""\n"
                               R"(            },)""\n"
                               R"(            {)""\n"
                               R"(                "description": "Danger",)""\n"
                               R"(                "solution": "Keep out",)""\n"
                               R"(                "state": "warning")""\n"
                               R"(            })""\n"
                               R"(        ],)""\n"
                               R"(        "state": "error")""\n"
                               R"(    })""\n"
                               R"(])""\n"
                               R"()");
        QTest::newRow("mixed") << writer << expectedOutput;
    }
}

void test_DiagnosisTestResultWriter::write_ShouldWriteTestResultsToAnIODevice()
{
    QFETCH(DiagnosisTestResultWriter, writer);
    QFETCH(QString, expectedOutput);

#ifdef Q_OS_WIN
    expectedOutput.replace("\n", "\r\n");
#endif

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    writer.write(buffer);

    QCOMPARE(QString(byteArray), expectedOutput);
}

DECLARE_TEST(test_DiagnosisTestResultWriter)

#include "test_diagnosistestresultwriter.moc"
