#include "testingportinuse.h"

namespace testing {

bool testing::TestingPortInUse::isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString)
{
    Q_UNUSED(port);
    serverError = m_testingServerError;
    errorString = m_testingErrorString;
    return m_testingResult;
}

}
