#include "studytesthelper.h"

#include "study.h"
#include "patient.h"
#include "seriestesthelper.h"

using namespace udg;

namespace testing {

Study* StudyTestHelper::createStudy(int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    return createStudyByUID("1", numberOfSeriesToAddToStudy, numberOfImagesToAddToSeries);
}

Study* StudyTestHelper::createStudy(int numberOfSeries, QVector<int> numberOfImagesInEachSeries)
{
    Study *study = new Study();

    study->setInstanceUID("1");

    for (int index = 0; index < numberOfSeries; index++)
    {
        Series *series = SeriesTestHelper::createSeriesByUID(QString::number(index), numberOfImagesInEachSeries[index]);
        series->setSeriesNumber(QString::number(index));
        study->addSeries(series);
    }

    return study;
}

Study* StudyTestHelper::createStudyByUID(QString instanceUID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        Series *series = SeriesTestHelper::createSeriesByUID(QString::number(index), numberOfImagesToAddToSeries);
        series->setSeriesNumber(QString::number(index));
        study->addSeries(series);
    }

    return study;
}

Study *StudyTestHelper::createStudyWithDescriptionAndSeriesModality(QString description, QString modality, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = createStudyByUID("1", numberOfSeriesToAddToStudy, numberOfImagesToAddToSeries);
    study->setDescription(description);

    foreach (Series *series, study->getSeries())
    {
        series->setModality(modality);
    }

    return study;
}

void StudyTestHelper::cleanUp(Study *study)
{
    if (study->getParentPatient() != NULL)
    {
        delete study->getParentPatient();
    }
    else
    {
        delete study;
    }
}

}
