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

HangingProtocol* HangingProtocolTestHelper::createHangingProtocolWithAttributes(QString name, int priority, bool strictness, bool allDifferent, bool previous, int identifier, int imageSets, int displaySets)
{
    HangingProtocol *testHangingProtocol = createEmptyHangingProtocol();
    testHangingProtocol->setName(name);
    testHangingProtocol->setIdentifier(identifier);
    testHangingProtocol->setPriority(priority);
    testHangingProtocol->setStrictness(strictness);
    testHangingProtocol->setAllDiferent(allDifferent);
    testHangingProtocol->setPrevious(previous);

    for(int i = 0; i < imageSets; i++)
    {
        HangingProtocolImageSet *imageSet = HangingProtocolImageSetTestHelper::createHangingProtocolImageSet();
        imageSet->setIdentifier(i+1);
        testHangingProtocol->addImageSet(imageSet);
    }

    for(int i = 0; i < displaySets; i++)
    {
        HangingProtocolDisplaySet *displaySet = HangingProtocolDisplaySetTestHelper::createHangingProtocolDisplaySet();
        displaySet->setIdentifier(i+1);
        testHangingProtocol->addDisplaySet(displaySet);
    }

    return testHangingProtocol;
}
}