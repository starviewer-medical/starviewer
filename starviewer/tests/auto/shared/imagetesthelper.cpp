#include "imagetesthelper.h"

#include "image.h"

using namespace udg;

namespace testing {

Image* ImageTestHelper::createImageByID(QString SOPInstanceUID)
{
    Image *testImage = new Image();

    testImage->setSOPInstanceUID(SOPInstanceUID);

    return testImage;
}

}
