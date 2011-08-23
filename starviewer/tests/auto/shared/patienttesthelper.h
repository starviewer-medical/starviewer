#ifndef PATIENTTESTHELPER_H
#define PATIENTTESTHELPER_H

#include <QString>

namespace udg {
    class Patient;
}

namespace testing {

class PatientTestHelper
{
public:
    /// Torna un Patient amd ID 1 amb amb el número d'estudis, de series i imatges en cada sèrie indicat.
    static udg::Patient* create(int numberOfStudies, int numberOfSeries = 0, int numberOfImages = 0);

    static udg::Patient* createPatientWithIDAndName(const QString &id, const QString &name);
};

}

#endif // PATIENTTESTHELPER_H
