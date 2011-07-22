#include "seriestesthelper.h"

#include "series.h"
#include "imagetesthelper.h"

namespace udg {

Series* SeriesTestHelper::getTestSeries(QString seriesInstanceUID, QString seriesNumber, int numberOfImages)
{
    Series *series = new Series();

    series->setInstanceUID(seriesInstanceUID);
    series->setSeriesNumber(seriesNumber);

    for (int index = 0; index < numberOfImages; index++)
    {
        series->addImage(ImageTestHelper::getTestImage(QString::number(index), QString::number(index)));
    }

    return series;
}

}
