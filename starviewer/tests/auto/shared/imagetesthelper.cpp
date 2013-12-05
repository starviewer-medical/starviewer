#include "imagetesthelper.h"

#include "seriestesthelper.h"
#include "patienttesthelper.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

using namespace udg;

namespace testing {

Image* ImageTestHelper::createImageByUID(QString SOPInstanceUID)
{
    Image *testImage = new Image();

    testImage->setSOPInstanceUID(SOPInstanceUID);

    return testImage;
}

Image *ImageTestHelper::createImageWithSexAndHeightAndWeight(QString sex, double height, int weight)
{
    Patient *patient = PatientTestHelper::create(1, 1, 1);
    Study *study = patient->getStudies().first();
    Series *series = study->getSeries().first();

    patient->setSex(sex);
    study->setHeight(height);
    study->setWeight(weight);

    return series->getImages().first();
}

Image* ImageTestHelper::createImageWithHeightAndWeight(double height, int weight)
{
    Patient *patient = PatientTestHelper::create(1, 1, 1);
    Study *study = patient->getStudies().first();
    Series *series = study->getSeries().first();

    study->setHeight(height);
    study->setWeight(weight);

    return series->getImages().first();
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
