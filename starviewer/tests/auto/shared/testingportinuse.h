#ifndef TESTINGPORTINUSE_H
#define TESTINGPORTINUSE_H

#include "portinuse.h"

#include <QAbstractSocket>
#include <QString>

namespace testing {

class TestingPortInUse : public udg::PortInUse {
public:
    bool m_testingResult;
    QAbstractSocket::SocketError m_testingServerError;
    QString m_testingErrorString;
    
protected:
    virtual bool isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString);
};

}
#endif // TESTINGPORTINUSE_H
