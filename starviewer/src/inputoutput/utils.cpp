#include "utils.h"

#include <QTcpServer>

#include "logging.h"


namespace udg{


bool Utils::isPortInUse(int port)
{
    QTcpServer tcpServer;
    bool portInUse;

    portInUse = !tcpServer.listen(QHostAddress::Any, port);

    if (!portInUse) 
    {
        tcpServer.close();
    }
    //No s'hauria de donar un error diferent a aquest, de totes maneres per seguretat el loggagem
    else if (tcpServer.serverError() != QAbstractSocket::AddressInUseError) 
    {
        ERROR_LOG("No s'ha pogut comprovat correctament si el port " + QString().setNum(port) + " està en ús, per error: " + tcpServer.errorString());
    }

    return portInUse;
}

}; //end udg namespace
