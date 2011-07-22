#include "seriestesthelper.h"

#include "imagetesthelper.h"
#include "series.h"

using namespace udg;

namespace testing {

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

}
