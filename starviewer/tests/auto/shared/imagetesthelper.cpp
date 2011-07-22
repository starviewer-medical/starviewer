#include "imagetesthelper.h"

#include "image.h"

namespace udg{

Image* udg::ImageTestHelper::getTestImage(QString instanceNumber, QString SOPInstanceUID)
{
    Image *testImage = new Image();

    testImage->setInstanceNumber(instanceNumber);
    testImage->setSOPInstanceUID(SOPInstanceUID);

    return testImage;
}

}

