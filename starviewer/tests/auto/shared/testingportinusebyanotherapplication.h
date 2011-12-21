#ifndef TESTINGPORTINUSEBYANOTHERAPPLICATION_H
#define TESTINGPORTINUSEBYANOTHERAPPLICATION_H

#include "portinusebyanotherapplication.h"

namespace testing {

class TestingPortInUseByAnotherApplication : public udg::PortInUseByAnotherApplication {
public:
    bool m_testingInUseByAnotherApplication;
public:
    TestingPortInUseByAnotherApplication();

    bool isPortInUseByAnotherApplication(int port, bool &error);
};

}
#endif // TESTINGPORTINUSE_H
