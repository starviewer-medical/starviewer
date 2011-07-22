#include "studytesthelper.h"

#include "study.h"
#include "seriestesthelper.h"

namespace udg {

Study* StudyTestHelper::getTestStudy(QString instanceUID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        study->addSeries(SeriesTestHelper::getTestSeries(QString::number(index), numberOfImagesToAddToSeries));
    }

    return study;
}

}
