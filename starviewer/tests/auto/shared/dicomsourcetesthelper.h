#ifndef DICOMSOURCETESTHELPER_H
#define DICOMSOURCETESTHELPER_H

#include <QString>

namespace udg {
    class DICOMSource;
}

namespace testing {

class DICOMSourceTestHelper
{
public:
    static udg::DICOMSource createAndAddPACSByID(QString pacsID);
};

}

#endif // DICOMSOURCETESTHELPER_H
