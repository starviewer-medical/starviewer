#include "studytesthelper.h"

#include "study.h"
#include "seriestesthelper.h"

namespace udg {

Study* StudyTestHelper::getTestStudy(QString instanceUID, QString studyID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);
    study->setID(studyID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        study->addSeries(SeriesTestHelper::getTestSeries(QString::number(index), QString::number(index), numberOfImagesToAddToSeries));
    }

    return study;
}

}
