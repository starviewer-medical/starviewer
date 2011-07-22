#include "imagetesthelper.h"

#include "seriestesthelper.h"
#include "image.h"

using namespace udg;

namespace testing {

Image* ImageTestHelper::createImageByID(QString SOPInstanceUID)
{
    Image *testImage = new Image();

    testImage->setSOPInstanceUID(SOPInstanceUID);

    return testImage;
}

void ImageTestHelper::cleanUp(Image *image)
{
    if (image->getParentSeries() != NULL)
    {
        SeriesTestHelper::cleanUp(image->getParentSeries());
    }
    else
    {
        delete image;
    }
}

}
