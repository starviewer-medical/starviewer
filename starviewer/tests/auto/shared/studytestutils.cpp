#include "studytestutils.h"

#include "study.h"
#include "seriestestutils.h"

namespace udg {

Study * StudyTestUtils::getTestStudy(QString instanceUID, QString studyID, int numberOfSeriesToAddToStudy, int numberOfImagesToAddToSeries)
{
    Study *study = new Study();

    study->setInstanceUID(instanceUID);
    study->setID(studyID);

    for (int index = 0; index < numberOfSeriesToAddToStudy; index++)
    {
        study->addSeries(SeriesTestUtils::getTestSeries(QString::number(index), QString::number(index), numberOfImagesToAddToSeries));
    }

    return study;
}

}
