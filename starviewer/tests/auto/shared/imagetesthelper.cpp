#include "imagetesthelper.h"

#include "image.h"

namespace udg{

Image* udg::ImageTestHelper::createImageByID(QString SOPInstanceUID)
{
    Image *testImage = new Image();

    testImage->setSOPInstanceUID(SOPInstanceUID);

    return testImage;
}

}

