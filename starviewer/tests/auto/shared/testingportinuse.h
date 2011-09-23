#ifndef TESTINGPORTINUSE_H
#define TESTINGPORTINUSE_H

#include "portinuse.h"

#include <QAbstractSocket>
#include <QString>

namespace testing {

class TestingPortInUse : public udg::PortInUse {
public:
    /// Atributs per modificar el resultat de isPortAvailable
    bool m_testingResult;
    QAbstractSocket::SocketError m_testingServerError;
    QString m_testingErrorString;

    /// Atributs per modificar el resultat de isPortInUse
    PortInUse::PortInUseStatus m_testingStatus;
    QString m_testingStatusErrorString;

    void setStatus(PortInUse::PortInUseStatus status);
    void setErrorString(QString errorString);
    
protected:
    virtual bool isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString);
};

}
#endif // TESTINGPORTINUSE_H
