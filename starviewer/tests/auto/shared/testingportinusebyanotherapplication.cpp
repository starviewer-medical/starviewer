#include "testingportinusebyanotherapplication.h"
#include <QObject>

namespace testing {

TestingPortInUseByAnotherApplication::TestingPortInUseByAnotherApplication()
{
}

bool TestingPortInUseByAnotherApplication::isPortInUseByAnotherApplication(int port, bool &error)
{
    Q_UNUSED(port);
    error = false;
    return m_testingInUseByAnotherApplication;
}

}
