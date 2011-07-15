#include "seriestestutils.h"

#include "series.h"
#include "imagetestutils.h"

namespace udg {

Series * SeriesTestUtils::getTestSeries(QString seriesInstanceUID, QString seriesNumber, int numberOfImages)
{
    Series *series = new Series();

    series->setInstanceUID(seriesInstanceUID);
    series->setSeriesNumber(seriesNumber);

    for (int index = 0; index < numberOfImages; index++)
    {
        series->addImage(ImageTestUtils::getTestImage(QString::number(index), QString::number(index)));
    }

    return series;
}

}
