#ifndef TESTINGPORTINUSE_H
#define TESTINGPORTINUSE_H

#include "portinuse.h"

#include <QAbstractSocket>
#include <QString>

namespace udg {
class PortInUseByAnotherApplication;
}

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

    /// Atributs per modificar el resultat de isPortInUseByAnotherApplication
    udg::PortInUseByAnotherApplication* m_testingPortInUseByAnotherApplication;

    void setStatus(PortInUse::PortInUseStatus status);
    void setErrorString(const QString &errorString);
    
protected:
    virtual bool isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString);
    virtual udg::PortInUseByAnotherApplication* createPortInUseByAnotherApplication(); 
};

}
#endif // TESTINGPORTINUSE_H
