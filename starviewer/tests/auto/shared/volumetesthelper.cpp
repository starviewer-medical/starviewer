#include "volumetesthelper.h"

#include "image.h"
#include "imagetesthelper.h"
#include "volumepixeldata.h"
#include "volume.h"
#include <vtkImageData.h>


using namespace udg;

namespace testing {

Volume* VolumeTestHelper::createVolume(int numberOfImages, int numberOfPhases, int numberOfSlicesPerPhase)
{
    Volume *testVolume = new Volume();
    testVolume->setNumberOfPhases(numberOfPhases);
    testVolume->setNumberOfSlicesPerPhase(numberOfSlicesPerPhase);

    for (int index = 0; index < numberOfImages; index++)
    {
        Image *image = ImageTestHelper::createImageByUID(QString::number(index));
        image->setPath(QString("C:\\Test\\path\\%1").arg(index));
        testVolume->addImage(image);
    }

    vtkImageData *testVtkImageData = vtkImageData::New();
    testVolume->setData(testVtkImageData);

    return testVolume;
}

Volume* VolumeTestHelper::createVolumeWithParameters(int numberOfImages, int numberOfPhases, int numberOfSlicesPerPhase, double origin[3], double spacing[3], int dimensions[3], int extent[6])
{
    Volume *testVolume = new Volume();
    testVolume->setNumberOfPhases(numberOfPhases);
    testVolume->setNumberOfSlicesPerPhase(numberOfSlicesPerPhase);

    for (int index = 0; index < numberOfImages; index++)
    {
        Image *image = ImageTestHelper::createImageByUID(QString::number(index));
        image->setPath(QString("C:\\Test\\path\\%1").arg(index));
        testVolume->addImage(image);
    }

    vtkImageData *testVtkImageData = vtkImageData::New();
    testVtkImageData->SetOrigin(origin);
    testVtkImageData->SetSpacing(spacing);
    testVtkImageData->SetDimensions(dimensions);
    testVtkImageData->SetWholeExtent(extent);
    testVolume->setData(testVtkImageData);

    return testVolume;
}

Volume* VolumeTestHelper::createMultiframeVolume(int numberOfImages, int numberOfPhases, int numberOfSlicesPerPhase)
{
    Volume *testVolume = new Volume();
    testVolume->setNumberOfPhases(numberOfPhases);
    testVolume->setNumberOfSlicesPerPhase(numberOfSlicesPerPhase);

    for (int index = 0; index < numberOfImages; index++)
    {
        Image *image = ImageTestHelper::createImageByUID(QString::number(index));
        image->setPath("C:\\Test\\path");
        testVolume->addImage(image);
    }

    vtkImageData *testVtkImageData = vtkImageData::New();
    testVolume->setData(testVtkImageData);

    return testVolume;
}

void VolumeTestHelper::cleanUp(Volume *volume)
{
    QList<Image*> images = volume->getImages();
    foreach (Image *image, images)
    {
        ImageTestHelper::cleanUp(image);
    }

    delete volume;
}
}
