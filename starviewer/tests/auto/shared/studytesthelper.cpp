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

Study* StudyTestHelper::createStudyByUID(QString instanceUID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        study->addSeries(SeriesTestHelper::createSeriesByUID(QString::number(index), numberOfImagesToAddToSeries));
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
