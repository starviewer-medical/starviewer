#include "portinuse.h"

#include <QTcpServer>

#include "logging.h"

namespace udg {

PortInUse::PortInUse()
{
    m_status = PortInUse::PortCheckError;
    m_errorString = "No port checked yet";
}

bool PortInUse::isPortInUse(int port)
{
    QTcpServer tcpServer;
    bool portInUse;

    portInUse = !tcpServer.listen(QHostAddress::Any, port);

    if (!portInUse)
    {
        m_status = PortInUse::PortIsAvailable;
        tcpServer.close();
    }
    else if (tcpServer.serverError() == QAbstractSocket::AddressInUseError)
    {
        m_status = PortInUse::PortIsInUse;
    }
    else
    {
        // No s'hauria de donar un error diferent a AddressInUseError, de totes maneres per seguretat el loggagem
        ERROR_LOG("No s'ha pogut comprovat correctament si el port " + QString().setNum(port) + " està en ús, per error: " + tcpServer.errorString());
        m_errorString = tcpServer.errorString();
    }

    return portInUse;
}

PortInUse::PortInUseStatus PortInUse::getStatus()
{
    return m_status;
}

QString PortInUse::getErrorString()
{
    return m_errorString;
}

} // End udg namespace
