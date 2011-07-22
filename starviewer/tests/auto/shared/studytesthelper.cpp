#include "studytesthelper.h"

#include "study.h"
#include "seriestesthelper.h"

using namespace udg;

namespace testing {

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

}
