#include "patienttesthelper.h"

#include "patient.h"
#include "studytesthelper.h"

using namespace udg;

namespace testing {

Patient* PatientTestHelper::create(int numberOfStudies, int numberOfSeries, int numberOfImages)
{
    Patient *patient = new Patient();
    patient->setID("1");

    for (int index = 0; index < numberOfStudies; index++)
    {
        patient->addStudy(StudyTestHelper::createStudy(numberOfSeries, numberOfImages));
    }

    return patient;
}

}
