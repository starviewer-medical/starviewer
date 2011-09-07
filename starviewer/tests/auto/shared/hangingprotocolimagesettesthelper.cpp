#include "hangingprotocolimagesettesthelper.h"

#include "hangingprotocolimageset.h"

using namespace udg;

namespace testing {

HangingProtocolImageSet* HangingProtocolImageSetTestHelper::createHangingProtocolImageSet()
{
    HangingProtocolImageSet *testHangingProtocolImageSet = new HangingProtocolImageSet();

    return testHangingProtocolImageSet;
}


}