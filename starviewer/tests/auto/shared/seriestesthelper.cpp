#include "seriestesthelper.h"

#include "studytesthelper.h"
#include "imagetesthelper.h"
#include "series.h"
#include "mathtools.h"
#include "image.h"

using namespace udg;

namespace testing {

Series* SeriesTestHelper::createSeries(int numberOfImages)
{
    return createSeriesByUID("1", numberOfImages);
}

Series* SeriesTestHelper::createSeriesByUID(QString seriesInstanceUID, int numberOfImages)
{
    Series *series = new Series();

    series->setInstanceUID(seriesInstanceUID);

    for (int index = 0; index < numberOfImages; index++)
    {
        series->addImage(ImageTestHelper::createImageByUID(QString::number(index)));
    }

    return series;
}

Series *SeriesTestHelper::createSeriesByModality(QString modality, int numberOfImages)
{
    Series *series = createSeriesByUID("1", numberOfImages);
    series->setModality(modality);

    if (numberOfImages > 0)
    {
        series->getImageByIndex(0)->setImageType("PRIMARY");
    }

    return series;
}

QString SeriesTestHelper::getRandomModality()
{
    QStringList modalities;
    modalities << "CT" << "MR" << "CR" << "MG" << "DX" << "US" << "NM" << "ES" << "PT" << "OP" << "XA" << "XC" << "RF" << "SC";
    return modalities.at(MathTools::randomInt(0, modalities.count() - 1));
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
