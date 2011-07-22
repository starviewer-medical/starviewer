#include "seriestesthelper.h"

#include "series.h"
#include "imagetesthelper.h"

namespace udg {

Series* SeriesTestHelper::getTestSeries(QString seriesInstanceUID, int numberOfImages)
{
    Series *series = new Series();

    series->setInstanceUID(seriesInstanceUID);

    for (int index = 0; index < numberOfImages; index++)
    {
        series->addImage(ImageTestHelper::getTestImage(QString::number(index)));
    }

    return series;
}

}
