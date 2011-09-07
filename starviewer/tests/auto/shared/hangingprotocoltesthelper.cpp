#include "hangingprotocoltesthelper.h"

#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocolimagesettesthelper.h"
#include "hangingprotocoldisplaysettesthelper.h"

using namespace udg;

namespace testing {

HangingProtocol* HangingProtocolTestHelper::createEmptyHangingProtocol()
{
    return new HangingProtocol();
}

HangingProtocol* HangingProtocolTestHelper::createHangingProtocolWithoutImageSetAndDisplaySet()
{
    HangingProtocol *testHangingProtocol = createEmptyHangingProtocol();
    testHangingProtocol->setStrictness(true);
    testHangingProtocol->setAllDiferent(true);
    testHangingProtocol->setPrevious(true);
    testHangingProtocol->setPriority(10.0);
    testHangingProtocol->setName(QString("HangingProtocolName"));
    testHangingProtocol->setIdentifier(1);

    return testHangingProtocol;
}

HangingProtocol* HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet()
{
    HangingProtocol *testHangingProtocol = createHangingProtocolWithoutImageSetAndDisplaySet();
    HangingProtocolImageSet *imageSet = HangingProtocolImageSetTestHelper::createHangingProtocolImageSet();
    imageSet->setIdentifier(1);
    HangingProtocolDisplaySet *displaySet = HangingProtocolDisplaySetTestHelper::createHangingProtocolDisplaySet();
    displaySet->setIdentifier(1);
    displaySet->setImageSet(imageSet);
    testHangingProtocol->addImageSet(imageSet);
    testHangingProtocol->addDisplaySet(displaySet);
    return testHangingProtocol;
}

}