/*@
    "name": "test_EchoToPACSTest",
    "requirements": ["archive.dimse", "archive.wado"]
 */

#include "autotest.h"
#include "echotopacstest.h"

#include "pacsdevice.h"

using namespace udg;

typedef QHash<QString, DiagnosisTestProblem> EchoResultHash;

class TestingEchoToPACSTest : public EchoToPACSTest {
public:
    QList<PacsDevice> m_pacsDevices;
    EchoResultHash m_resultHash;

protected:
    QList<PacsDevice> getPacsDeviceList() override
    {
        return m_pacsDevices;
    }

    DiagnosisTestProblem echo(const PacsDevice &pacs) override
    {
        return m_resultHash[pacs.getID()];
    }
};

class test_EchoToPACSTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfPACSAreAccessible_data();
    void run_ShouldTestIfPACSAreAccessible();
};

Q_DECLARE_METATYPE(QList<PacsDevice>)
Q_DECLARE_METATYPE(EchoResultHash)
Q_DECLARE_METATYPE(DiagnosisTestResult)

void test_EchoToPACSTest::run_ShouldTestIfPACSAreAccessible_data()
{
    QTest::addColumn<QList<PacsDevice>>("pacsDevices");
    QTest::addColumn<EchoResultHash>("echoResultHash");
    QTest::addColumn<DiagnosisTestResult>("expectedDiagnosisTestResult");

    {
        DiagnosisTestResult result;
        result.addWarning(DiagnosisTestProblem(DiagnosisTestProblem::Warning, "There are no PACS defined",
                                               "New PACS can be defined at Tools > Configuration > PACS"));

        QTest::newRow("no pacs") << QList<PacsDevice>{} << EchoResultHash{} << result;
    }

    {
        PacsDevice pacs;
        pacs.setID("0");
        pacs.setType(PacsDevice::Type::Dimse);
        pacs.setAETitle("PACS");

        QTest::newRow("one pacs working") << QList<PacsDevice>{pacs} << EchoResultHash{{"0", DiagnosisTestProblem(DiagnosisTestProblem::Ok)}}
                                          << DiagnosisTestResult();
    }

    {
        PacsDevice pacs;
        pacs.setID("0");
        pacs.setType(PacsDevice::Type::Dimse);
        pacs.setAETitle("PACS");
        DiagnosisTestProblem problem(DiagnosisTestProblem::Error, "Echo to PACS with AE Title 'PACS' failed",
                                     "Check PACS configuration at Tools > Configuration > PACS");
        DiagnosisTestResult result;
        result.addError(problem);

        QTest::newRow("one pacs fail") << QList<PacsDevice>{pacs} << EchoResultHash{{"0", problem}} << result;
    }

    {
        PacsDevice pacs0;
        pacs0.setID("0");
        pacs0.setType(PacsDevice::Type::Dimse);
        pacs0.setAETitle("PACS0");
        PacsDevice pacs1;
        pacs1.setID("1");
        pacs1.setType(PacsDevice::Type::WadoUriDimse);
        pacs1.setAETitle("PACS1");
        pacs1.setBaseUri(QUrl("http://pacs1.local/wado-uri"));
        PacsDevice pacs2;
        pacs2.setID("2");
        pacs2.setType(PacsDevice::Type::Dimse);
        pacs2.setAETitle("PACS2");
        PacsDevice pacs3;
        pacs3.setID("3");
        pacs3.setType(PacsDevice::Type::Wado);
        pacs3.setBaseUri(QUrl("http://example.com"));
        DiagnosisTestProblem problem0(DiagnosisTestProblem::Error, "Echo to PACS with AE Title 'PACS0' failed",
                                      "Check PACS configuration at Tools > Configuration > PACS");
        DiagnosisTestProblem problem1(DiagnosisTestProblem::Ok);
        DiagnosisTestProblem problem2(DiagnosisTestProblem::Error, "Unable to connect to PACS with AE Title 'PACS2'",
                                      "Check internet connection and PACS configuration at Tools > Configuration > PACS");
        DiagnosisTestProblem problem3(DiagnosisTestProblem::Error, "Connection to PACS with base URI 'http://example.com' failed",
                                      "Check internet connection and PACS configuration at Tools > Configuration > PACS");
        DiagnosisTestResult result;
        result.addError(problem0);
        result.addError(problem2);
        result.addError(problem3);

        QTest::newRow("4 pacs: fail, ok, connect error, wado error") << QList<PacsDevice>{pacs0, pacs1, pacs2, pacs3}
                                                                     << EchoResultHash{{"0", problem0}, {"1", problem1}, {"2", problem2}, {"3", problem3}}
                                                                     << result;
    }
}

void test_EchoToPACSTest::run_ShouldTestIfPACSAreAccessible()
{
    QFETCH(QList<PacsDevice>, pacsDevices);
    QFETCH(EchoResultHash, echoResultHash);
    QFETCH(DiagnosisTestResult, expectedDiagnosisTestResult);

    TestingEchoToPACSTest echoToPACSTest;
    echoToPACSTest.m_pacsDevices = pacsDevices;
    echoToPACSTest.m_resultHash = echoResultHash;

    DiagnosisTestResult result = echoToPACSTest.run();

    QCOMPARE(result.getState(), expectedDiagnosisTestResult.getState());
    if (result.getState() != DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getErrors().size(), expectedDiagnosisTestResult.getErrors().size());
        QCOMPARE(result.getWarnings().size(), expectedDiagnosisTestResult.getWarnings().size());

        QListIterator<DiagnosisTestProblem> resultIterator(result.getErrors() + result.getWarnings());
        QListIterator<DiagnosisTestProblem> testingResultIterator(expectedDiagnosisTestResult.getErrors() + expectedDiagnosisTestResult.getWarnings());
        while (resultIterator.hasNext() && testingResultIterator.hasNext())
        {
            DiagnosisTestProblem problem = resultIterator.next();
            DiagnosisTestProblem testingProblem = testingResultIterator.next();

            QCOMPARE(problem.getDescription(), testingProblem.getDescription());
            QCOMPARE(problem.getSolution(), testingProblem.getSolution());
        }
    }
}

DECLARE_TEST(test_EchoToPACSTest)

#include "test_echotopacstest.moc"
