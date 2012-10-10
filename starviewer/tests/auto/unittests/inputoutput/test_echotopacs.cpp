#include "autotest.h"
#include "echotopacs.h"
#include "pacsdevice.h"

#include <ofcond.h>

using namespace udg;

Q_DECLARE_METATYPE(EchoToPACS::EchoRequestStatus)

class TestingEchoToPACS : public EchoToPACS {
public:
    bool m_connectToPACSResult;
    bool m_echoToPACSResult;

protected:
    virtual bool connectToPACS(PACSConnection *pacsConnection)
    {
        Q_UNUSED(pacsConnection);
        return m_connectToPACSResult;
    }

    virtual OFCondition echoToPACS(PACSConnection *pacsConnection)
    {
        Q_UNUSED(pacsConnection);

        OFStatus status;
        if (m_echoToPACSResult)
        {
            status = OF_ok;
        }
        else
        {
            status = OF_failure;
        }

        OFCondition condition(0, 0, status, "");

        return condition;
    }
    virtual void disconnectFromPACS(PACSConnection *pacsConnection)
    {
        Q_UNUSED(pacsConnection);
    }
};

class test_EchoToPACS : public QObject {
Q_OBJECT

private slots:
    void echo_ShouldEchoAPacs_data();
    void echo_ShouldEchoAPacs();
};

void test_EchoToPACS::echo_ShouldEchoAPacs_data()
{
    /// Resultats del test
    QTest::addColumn<EchoToPACS::EchoRequestStatus>("status");
    QTest::addColumn<bool>("echoOk");

    /// Variables d'entrada
    QTest::addColumn<bool>("connectToPacsResult");
    QTest::addColumn<bool>("echoToPacsResult");

    /// Per definir que algun paràmetre no s'utilitza en el test, per tant és indiferent que s'hi posi
    bool unusedBool = true;

    /// Dades del test
    QTest::newRow("pacs working") << EchoToPACS::EchoOk << true << true << true;
    QTest::newRow("echo failed") << EchoToPACS::EchoFailed << false << true << false;
    QTest::newRow("connection error") << EchoToPACS::EchoCanNotConnectToPACS << false << false << unusedBool;
}

void test_EchoToPACS::echo_ShouldEchoAPacs()
{
    QFETCH(EchoToPACS::EchoRequestStatus, status);
    QFETCH(bool, echoOk);

    QFETCH(bool, connectToPacsResult);
    QFETCH(bool, echoToPacsResult);

    TestingEchoToPACS echoToPACS;
    echoToPACS.m_connectToPACSResult = connectToPacsResult;
    echoToPACS.m_echoToPACSResult = echoToPacsResult;

    /// El pacs device pot ser buit, el test no l'utilitza
    PacsDevice pacs;
    QCOMPARE(echoToPACS.echo(pacs), echoOk);
    QCOMPARE(echoToPACS.getLastError(), status);
}

DECLARE_TEST(test_EchoToPACS)

#include "test_echotopacs.moc"

