#include "dicomsourcetesthelper.h"

#include "pacsdevicetesthelper.h"
#include "dicomsource.h"

using namespace udg;

namespace testing {

DICOMSource DICOMSourceTestHelper::createAndAddPACSByID(QString pacsID)
{
    DICOMSource dicomSource;

    dicomSource.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID(pacsID));

    return dicomSource;
}

}
