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
Q_DECLARE_METATYPE(DiagnosisTestResult::DiagnosisTestResultState)
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
    QTest::addColumn<DiagnosisTestResult::DiagnosisTestResultState>("testingDiagnosisTestResultState");
    QTest::addColumn<QString>("testingDiagnosisTestResultDescription");
    QTest::addColumn<QString>("testingDiagnosisTestResultSolution");

    /// Variables unused
    QString unusedString = "";
    QList<PacsDevice> unusedPacsList;
    EchoRequestStatusHash unusedHash;

    /// Dades dels tests
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
    
    // Dades del test 4
    QList<QString> AETitlesTest4;
    QList<EchoToPACS::EchoRequestStatus> statusesTest4;
    QList<PacsDevice> pacsListTest4;
    EchoRequestStatusHash hashTest4;
    
    AETitlesTest4 << "PACSGUELL" << "PACSLLEIDA" << "RS_HGJT";
    statusesTest4 << EchoToPACS::EchoFailed << EchoToPACS::EchoOk << EchoToPACS::EchoCanNotConnectToPACS;
    fillPacsListAndHash(&pacsListTest4, &hashTest4, AETitlesTest4, statusesTest4);

    /// Tests
    QTest::newRow("no pacs") << unusedPacsList << unusedHash << DiagnosisTestResult::Warning << "There are no PACS defined" << unusedString;
    QTest::newRow("one pacs working") << pacsListTest2 << hashTest2 << DiagnosisTestResult::Ok << unusedString << unusedString;
    QTest::newRow("one pacs fail") << pacsListTest3 << hashTest3
                                   << DiagnosisTestResult::Error
                                   << "Echo to pacs with AE Title '" + AETitlesTest3[0] + "' failed"
                                   << "Contact PACS supervisor";
    QTest::newRow("3 pacs: fail, ok, connect error") << pacsListTest4 << hashTest4
                                                 << DiagnosisTestResult::Error 
                                                 << "Echo to pacs with AE Title '" + AETitlesTest4[0] +
                                                    "' failed\nUnable to connect to PACS with AE Title '" + AETitlesTest4[2] + "'" 
                                                 << "Contact PACS supervisor\nCheck PACS URL, or internet connection";
}

void test_EchoToPACSTest::run_ShouldTestIfPACSAreAccessible()
{
    QFETCH(QList<PacsDevice>, testingPacsDevices);
    QFETCH(EchoRequestStatusHash, testingEchoRequestStatusHash);
 
    QFETCH(DiagnosisTestResult::DiagnosisTestResultState, testingDiagnosisTestResultState);
    QFETCH(QString, testingDiagnosisTestResultDescription);
    QFETCH(QString, testingDiagnosisTestResultSolution);

    TestingEchoToPACSTest echoToPACSTest;
    
    echoToPACSTest.m_pacsDevices = testingPacsDevices;
    echoToPACSTest.m_statusHash = testingEchoRequestStatusHash;

    DiagnosisTestResult result = echoToPACSTest.run();

    QCOMPARE(result.getState(), testingDiagnosisTestResultState);
    if (result.getState()!= DiagnosisTestResult::Ok)
    {
        QCOMPARE(result.getDescription(), testingDiagnosisTestResultDescription);
        QCOMPARE(result.getSolution(), testingDiagnosisTestResultSolution);
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
