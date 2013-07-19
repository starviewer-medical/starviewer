#include "volumetesthelper.h"

#include "image.h"
#include "imagetesthelper.h"
#include "patient.h"
#include "patienttesthelper.h"
#include "series.h"
#include "study.h"
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
    Patient *patient = PatientTestHelper::create(1, 1, numberOfImages);
    testVolume->setImages(patient->getStudies().first()->getSeries().first()->getImages());

    for (int index = 0; index < numberOfImages; index++)
    {
        testVolume->getImages().at(index)->setPath(QString("C:\\Test\\path\\%1").arg(index));
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
    Patient *patient = PatientTestHelper::create(1, 1, numberOfImages);
    testVolume->setImages(patient->getStudies().first()->getSeries().first()->getImages());

    for (int index = 0; index < numberOfImages; index++)
    {
        testVolume->getImages().at(index)->setPath(QString("C:\\Test\\path\\%1").arg(index));
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
    Patient *patient = PatientTestHelper::create(1, 1, numberOfImages);
    testVolume->setImages(patient->getStudies().first()->getSeries().first()->getImages());

    for (int index = 0; index < numberOfImages; index++)
    {
        testVolume->getImages().at(index)->setPath(QString("C:\\Test\\path"));
    }

    vtkImageData *testVtkImageData = vtkImageData::New();
    testVolume->setData(testVtkImageData);

    return testVolume;
}

void VolumeTestHelper::cleanUp(Volume *volume)
{
    if (volume && volume->getImage(0))
    {
        // This calls cleanUp() on SeriesTestHelper with the parent series and deletes the series and all images
        ImageTestHelper::cleanUp(volume->getImage(0));
    }

    delete volume;
}
}
