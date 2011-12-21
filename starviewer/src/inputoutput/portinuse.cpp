#include "portinuse.h"
#include "portinusebyanotherapplication.h"

#include <QTcpServer>

#include "logging.h"

namespace udg {

PortInUse::PortInUse()
{
    m_status = PortInUse::PortUnknownStatus;
    m_errorString = QObject::tr("No port checked yet");
}

bool PortInUse::isPortInUse(int port)
{
    bool portInUse;
    QAbstractSocket::SocketError serverError;
    QString errorString;

    portInUse = !isPortAvailable(port, serverError, errorString);

    if (!portInUse)
    {
        m_status = PortInUse::PortIsAvailable;
    }
    else if (serverError == QAbstractSocket::AddressInUseError)
    {
        m_status = PortInUse::PortIsInUse;
    }
    else
    {
        // No s'hauria de donar un error diferent a AddressInUseError, de totes maneres per seguretat el loggagem
        ERROR_LOG("No s'ha pogut comprovat correctament si el port " + QString().setNum(port) + " està en ús, per error: " + errorString);
        m_errorString = errorString;
        m_status = PortInUse::PortCheckError;
    }

    m_lastPortChecked = port;
    return portInUse;
}

PortInUse::PortInUseOwner PortInUse::getOwner()
{
    PortInUse::PortInUseOwner owner = PortInUse::PortUsedByUnknown;

    // En cas que el port estigui en ús, cal mirar si l'ha obert Starviewer o una altra aplicació
    if (m_status == PortInUse::PortIsInUse)
    {
        // S'instancia un objecte de la calsse segons el sistema operatiu
        PortInUseByAnotherApplication *portInUse = createPortInUseByAnotherApplication();
        bool error;
        bool inUse = portInUse->isPortInUseByAnotherApplication(m_lastPortChecked, error);
        if (!error)
        {
            if (inUse)
            {
                // Port obert per una altra aplicació
                owner = PortInUse::PortUsedByOther;
            }
            else
            {
                // port obert per Starviewer
                owner = PortInUse::PortUsedByStarviewer;
            }
        }

        delete portInUse;
    }

    return owner;
}

bool PortInUse::isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString)
{
    QTcpServer tcpServer;
    bool result;

    /// Result serà cert si el port està lliure, pertant s'ha de retorna l'oposat
    result = tcpServer.listen(QHostAddress::Any, port);
    serverError = tcpServer.serverError();
    errorString = tcpServer.errorString();

    tcpServer.close();

    return result;
}

PortInUse::PortInUseStatus PortInUse::getStatus()
{
    return m_status;
}

QString PortInUse::getErrorString()
{
    return m_errorString;
}

udg::PortInUseByAnotherApplication* PortInUse::createPortInUseByAnotherApplication()
{
    return PortInUseByAnotherApplication::newInstance();
}

} // End udg namespace
