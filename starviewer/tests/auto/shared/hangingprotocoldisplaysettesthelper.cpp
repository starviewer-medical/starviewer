#include "hangingprotocoldisplaysettesthelper.h"

#include "hangingprotocoldisplayset.h"
using namespace udg;

namespace testing {

HangingProtocolDisplaySet* HangingProtocolDisplaySetTestHelper::createHangingProtocolDisplaySet()
{
    HangingProtocolDisplaySet *testHangingProtocolDisplaySet = new HangingProtocolDisplaySet();
    
    return testHangingProtocolDisplaySet;
}

}