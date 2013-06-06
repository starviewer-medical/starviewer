#include "testingpacsconnection.h"

#include "pacsdevice.h"

namespace testing {

TestingPACSConnection::TestingPACSConnection() :
    PACSConnection(PacsDevice())
{
}

bool TestingPACSConnection::connectToPACS(PACSServiceToRequest pacsServiceToRequest)
{
    Q_UNUSED(pacsServiceToRequest);
    return true;
}

}
