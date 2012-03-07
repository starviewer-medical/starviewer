#include "autotest.h"
#include "echotopacs.h"
#include "echotopacstest.h"
#include "diagnosistestresult.h"

#include <QList>
#include <QHash>

using namespace udg;

typedef QHash<QString, EchoToPACS::EchoRequestStatus> EchoRequestStatusHash;

class TestingEchoToPACSTest : public EchoToPACSTest {
public:
    QList<PacsDevice> m_pacsDevices;
    EchoRequestStatusHash m_statusHash;

protected:
    virtual QList<PacsDevice> getPacsDeviceList()
    {
        return m_pacsDevices;
    }

    virtual EchoToPACS::EchoRequestStatus echo(const PacsDevice &pacs)
    {
        return m_statusHash[pacs.getAETitle()];
    }
};

Q_DECLARE_METATYPE(EchoToPACS::EchoRequestStatus)
Q_DECLARE_METATYPE(DiagnosisTestResult)
Q_DECLARE_METATYPE(QList<PacsDevice>)
Q_DECLARE_METATYPE(EchoRequestStatusHash)

class test_EchoToPACSTest : public QObject {
Q_OBJECT

private slots:
    void run_ShouldTestIfPACSAreAccessible_data();
    void run_ShouldTestIfPACSAreAccessible();

private:
    void fillPacsListAndHash(QList<PacsDevice> *list, EchoRequestStatusHash *hash,
                             const QList<QString> &AETitles, const QList<EchoToPACS::EchoRequestStatus> &statuses);
};

void test_EchoToPACSTest::run_ShouldTestIfPACSAreAccessible_data()
{
    /// Entrada
    QTest::addColumn<QList<PacsDevice> >("testingPacsDevices");
    QTest::addColumn<EchoRequestStatusHash>("testingEchoRequestStatusHash");

    /// Sortida
    QTest::addColumn<DiagnosisTestResult>("testingDiagnosisTestResult");

    /// Variables unused
    QString unusedString = "";
    QList<PacsDevice> unusedPacsList;
    EchoRequestStatusHash unusedHash;

    /// Dades dels tests
    // Dades del text 1
    DiagnosisTestResult noPacsResult;
    DiagnosisTestProblem noPacsProblem(DiagnosisTestProblem::Warning, "There are no PACS defined", "New PACS can be defined at Tools > Configuration > PACS");
    noPacsResult.addWarning(noPacsProblem);

    // Dades del test 2
    QList<QString> AETitlesTest2;
    QList<EchoToPACS::EchoRequestStatus> statusesTest2;
    QList<PacsDevice> pacsListTest2;
    EchoRequestStatusHash hashTest2;

    AETitlesTest2 << "PACS";
    statusesTest2 << EchoToPACS::EchoOk;
    fillPacsListAndHash(&pacsListTest2, &hashTest2, AETitlesTest2, statusesTest2);

    // Dades del test 3
    QList<QString> AETitlesTest3;
    QList<EchoToPACS::EchoRequestStatus> statusesTest3;
    QList<PacsDevice> pacsListTest3;
    EchoRequestStatusHash hashTest3;
    
    AETitlesTest3 << "RS_HUAV";
    statusesTest3 << EchoToPACS::EchoFailed;
    fillPacsListAndHash(&pacsListTest3, &hashTest3, AETitlesTest3, statusesTest3);
    
    DiagnosisTestResult pacsFailResult;
    DiagnosisTestProblem pacsFailProblem(DiagnosisTestProblem::Error, QString("Echo to PACS with AE Title '%1' failed").arg(AETitlesTest3[0]), "Check PACS configuration at Tools > Configuration > PACS");
    pacsFailResult.addError(pacsFailProblem);

    // Dades del test 4
    QList<QString> AETitlesTest4;
    QList<EchoToPACS::EchoRequestStatus> statusesTest4;
    QList<PacsDevice> pacsListTest4;
    EchoRequestStatusHash hashTest4;
    
    AETitlesTest4 << "PACSGUELL" << "PACSLLEIDA" << "RS_HGJT";
    statusesTest4 << EchoToPACS::EchoFailed << EchoToPACS::EchoOk << EchoToPACS::EchoCanNotConnectToPACS;
    fillPacsListAndHash(&pacsListTest4, &hashTest4, AETitlesTest4, statusesTest4);

    DiagnosisTestResult test4Result;
    DiagnosisTestProblem failProblem(DiagnosisTestProblem::Error, QString("Echo to PACS with AE Title '%1' failed").arg(AETitlesTest4[0]), "Check PACS configuration at Tools > Configuration > PACS");
    test4Result.addError(failProblem);
    DiagnosisTestProblem failProblem2(DiagnosisTestProblem::Error, QString("Unable to connect to PACS with AE Title '%1'").arg(AETitlesTest4[2]), "Check internet connection and PACS configuration at Tools > Configuration > PACS");
    test4Result.addError(failProblem2);

    /// Tests
    QTest::newRow("no pacs") << unusedPacsList << unusedHash << noPacsResult;
    QTest::newRow("one pacs working") << pacsListTest2 << hashTest2 << DiagnosisTestResult();
    QTest::newRow("one pacs fail") << pacsListTest3 << hashTest3 << pacsFailResult;
    QTest::newRow("3 pacs: fail, ok, connect error") << pacsListTest4 << hashTest4 << test4Result;
}

void test_EchoToPACSTest::run_ShouldTestIfPACSAreAccessible()
{
    QFETCH(QList<PacsDevice>, testingPacsDevices);
    QFETCH(EchoRequestStatusHash, testingEchoRequestStatusHash);
 
    QFETCH(DiagnosisTestResult, testingDiagnosisTestResult);

    TestingEchoToPACSTest echoToPACSTest;
    
    echoToPACSTest.m_pacsDevices = testingPacsDevices;
    echoToPACSTest.m_statusHash = testingEchoRequestStatusHash;

    DiagnosisTestResult result = echoToPACSTest.run();

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

void test_EchoToPACSTest::fillPacsListAndHash(QList<PacsDevice> *list, EchoRequestStatusHash *hash,
                                              const QList<QString> &AETitles, const QList<EchoToPACS::EchoRequestStatus> &statuses)
{
    if (AETitles.count() != statuses.count())
    {
        return;
    }

    for (int i = 0; i < AETitles.count(); i++)
    {
        PacsDevice pacs;
        pacs.setAETitle(AETitles[i]);
        list->append(pacs);

        hash->insert(AETitles[i], statuses[i]);
    }
}

DECLARE_TEST(test_EchoToPACSTest)

#include "test_echotopacstest.moc"
