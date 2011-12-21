#include "autotest.h"
#include "portinuse.h"
#include "portinusebyanotherapplication.h"
#include "testingportinuse.h"
#include "testingportinusebyanotherapplication.h"

#include <QAbstractSocket>

using namespace udg;
using namespace testing;

Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
Q_DECLARE_METATYPE(PortInUse::PortInUseStatus)
Q_DECLARE_METATYPE(PortInUse::PortInUseOwner)

class test_PortInUse : public QObject {
Q_OBJECT

private slots:
    void isPortInUse_ShouldCheckIfPortIsInUse_data();
    void isPortInUse_ShouldCheckIfPortIsInUse();

    void isPortInUse_ShouldCheckIfPortIsInUseFailAndReturnError_data();
    void isPortInUse_ShouldCheckIfPortIsInUseFailAndReturnError();

    void isPortInUse_ShouldCheckIfPortIsInUseByAnotherApplication_data();
    void isPortInUse_ShouldCheckIfPortIsInUseByAnotherApplication();
};

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUse_data()
{
    QTest::addColumn<int>("port");
    QTest::addColumn<bool>("inUse");
    QTest::addColumn<PortInUse::PortInUseStatus>("status");
    QTest::addColumn<bool>("testingResult");
    QTest::addColumn<QAbstractSocket::SocketError>("testingServerError");

    QTest::newRow("free port") << 1111 << false << PortInUse::PortIsAvailable << true << QAbstractSocket::UnknownSocketError;
    QTest::newRow("port in use") << 1234 << true << PortInUse::PortIsInUse << false << QAbstractSocket::AddressInUseError;
}

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUse()
{
    QFETCH(int, port);
    QFETCH(bool, inUse);
    QFETCH(PortInUse::PortInUseStatus, status);
    QFETCH(bool, testingResult);
    QFETCH(QAbstractSocket::SocketError, testingServerError);

    TestingPortInUse portInUse;
    portInUse.m_testingResult = testingResult;
    portInUse.m_testingServerError = testingServerError;

    QCOMPARE(portInUse.isPortInUse(port), inUse);
    QCOMPARE(portInUse.getStatus(), status);
}

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUseFailAndReturnError_data()
{
    QTest::addColumn<int>("port");
    QTest::addColumn<bool>("inUse");
    QTest::addColumn<PortInUse::PortInUseStatus>("status");
    QTest::addColumn<QString>("errorString");
    QTest::addColumn<bool>("testingResult");
    QTest::addColumn<QAbstractSocket::SocketError>("testingServerError");
    QTest::addColumn<QString>("testingErrorString");

    QTest::newRow("network error") << 1111 << true << PortInUse::PortCheckError << "Network error" << false << QAbstractSocket::NetworkError << "Network error";
}

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUseFailAndReturnError()
{
    QFETCH(int, port);
    QFETCH(bool, inUse);
    QFETCH(PortInUse::PortInUseStatus, status);
    QFETCH(QString, errorString);
    QFETCH(bool, testingResult);
    QFETCH(QAbstractSocket::SocketError, testingServerError);
    QFETCH(QString, testingErrorString);

    TestingPortInUse portInUse;
    portInUse.m_testingResult = testingResult;
    portInUse.m_testingServerError = testingServerError;
    portInUse.m_testingErrorString = testingErrorString;

    QCOMPARE(portInUse.isPortInUse(port), inUse);
    QCOMPARE(portInUse.getStatus(), status);
    QCOMPARE(portInUse.getErrorString(), errorString);
}

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUseByAnotherApplication_data()
{
    // Sortida
    QTest::addColumn<PortInUse::PortInUseOwner>("owner");
    // Entrada
    QTest::addColumn<bool>("testingResult");
    QTest::addColumn<bool>("testingInUseByAnotherApplication");

    bool unusedBool = false;

    QTest::newRow("free port") << PortInUse::PortUsedByUnknown << true << unusedBool;
    QTest::newRow("port in use by starviewer") << PortInUse::PortUsedByStarviewer << false << false;
    QTest::newRow("port in use by another application") << PortInUse::PortUsedByOther << false << true;
}

void test_PortInUse::isPortInUse_ShouldCheckIfPortIsInUseByAnotherApplication()
{
    QFETCH(PortInUse::PortInUseOwner, owner);
    QFETCH(bool, testingResult);
    QFETCH(bool, testingInUseByAnotherApplication);

    TestingPortInUse portInUse;
    portInUse.m_testingResult = testingResult;
    if (!testingResult)
    {
        // Si la crida isPortInUse retorna que sí (available = fals), cal que l'error sigui AddressInUseError
        portInUse.m_testingServerError = QAbstractSocket::AddressInUseError;
    }
    TestingPortInUseByAnotherApplication *portInUseByAnotherApplication = new TestingPortInUseByAnotherApplication();
    portInUseByAnotherApplication->m_testingInUseByAnotherApplication = testingInUseByAnotherApplication;
    portInUse.m_testingPortInUseByAnotherApplication = portInUseByAnotherApplication;
    
    // El port a comprovar no importa, hi posem un 0
    portInUse.isPortInUse(0);
    QCOMPARE(portInUse.getOwner(), owner);
}

DECLARE_TEST(test_PortInUse)

#include "test_portinuse.moc"

