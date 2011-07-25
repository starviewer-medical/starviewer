#include "studytesthelper.h"

#include "study.h"
#include "patient.h"
#include "seriestesthelper.h"

using namespace udg;

namespace testing {

Study* StudyTestHelper::createStudy(int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    return createStudyByID("1", numberOfSeriesToAddToStudy, numberOfImagesToAddToSeries);
}

Study* StudyTestHelper::createStudyByID(QString instanceUID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        study->addSeries(SeriesTestHelper::createSeriesByID(QString::number(index), numberOfImagesToAddToSeries));
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
