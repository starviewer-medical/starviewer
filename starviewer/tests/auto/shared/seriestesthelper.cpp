#include "seriestesthelper.h"

#include "studytesthelper.h"
#include "imagetesthelper.h"
#include "series.h"

using namespace udg;

namespace testing {

Series* SeriesTestHelper::createSeries(int numberOfImages)
{
    return createSeriesByID("1", numberOfImages);
}

Series* SeriesTestHelper::createSeriesByID(QString seriesInstanceUID, int numberOfImages)
{
    Series *series = new Series();

    series->setInstanceUID(seriesInstanceUID);

    for (int index = 0; index < numberOfImages; index++)
    {
        series->addImage(ImageTestHelper::createImageByID(QString::number(index)));
    }

    return series;
}

void SeriesTestHelper::cleanUp(Series *series)
{
    if (series->getParentStudy() != NULL)
    {
        StudyTestHelper::cleanUp(series->getParentStudy());
    }
    else
    {
        delete series;
    }
}

}
