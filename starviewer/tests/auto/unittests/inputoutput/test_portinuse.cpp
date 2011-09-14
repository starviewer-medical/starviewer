#include "autotest.h"
#include "portinuse.h"

#include <QAbstractSocket>

using namespace udg;

class TestingPortInUse : public PortInUse {
public:
    int m_testingResult;
    QAbstractSocket::SocketError m_testingServerError;
    QString m_testingErrorString;
    
protected:

    virtual bool isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString)
    {
        Q_UNUSED(port);
        serverError = m_testingServerError;
        errorString = m_testingErrorString;
        return m_testingResult;
    }
};

Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
Q_DECLARE_METATYPE(PortInUse::PortInUseStatus)

class test_PortInUse : public QObject {
Q_OBJECT

private slots:
    void isPortInUse_ShouldCheckIfPortIsInUse_data();
    void isPortInUse_ShouldCheckIfPortIsInUse();

    void isPortInUse_ShouldFailAndReturnError_data();
    void isPortInUse_ShouldFailAndReturnError();
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

void test_PortInUse::isPortInUse_ShouldFailAndReturnError_data()
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

void test_PortInUse::isPortInUse_ShouldFailAndReturnError()
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

DECLARE_TEST(test_PortInUse)

#include "test_portinuse.moc"

