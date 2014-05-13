#ifndef TESTINGPACSCONNECTION_H
#define TESTINGPACSCONNECTION_H

#include "pacsconnection.h"

using namespace udg;

namespace testing {

class TestingPACSConnection : public PACSConnection {

public:

    TestingPACSConnection();

    virtual bool connectToPACS(PACSServiceToRequest pacsServiceToRequest);

};

}

#endif // TESTINGPACSCONNECTION_H
