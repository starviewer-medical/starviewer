#include "testingportinuse.h"

namespace testing {

bool TestingPortInUse::isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString)
{
    Q_UNUSED(port);
    serverError = m_testingServerError;
    errorString = m_testingErrorString;
    return m_testingResult;
}

void TestingPortInUse::setStatus(PortInUse::PortInUseStatus status)
{
    m_status = status;
}

void TestingPortInUse::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
}

udg::PortInUseByAnotherApplication* TestingPortInUse::createPortInUseByAnotherApplication()
{
    return m_testingPortInUseByAnotherApplication;
}

}
