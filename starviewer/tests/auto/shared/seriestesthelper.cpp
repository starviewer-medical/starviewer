#include "seriestesthelper.h"

#include "series.h"
#include "imagetesthelper.h"

namespace udg {

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
