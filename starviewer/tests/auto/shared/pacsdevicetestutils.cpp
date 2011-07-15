#include "pacsdevicetestutils.h"

namespace udg {

PacsDevice PACSDeviceTestUtils::getTestPACSDevice(QString ID)
{
    PacsDevice pacsDevice;

    pacsDevice.setID(ID);
    pacsDevice.setAddress(ID);
    pacsDevice.setAETitle(ID);
    pacsDevice.setQueryRetrieveServiceEnabled(true);
    pacsDevice.setQueryRetrieveServicePort(4006);
    pacsDevice.setInstitution(ID);
    pacsDevice.setDefault(true);
    pacsDevice.setDescription("");
    pacsDevice.setLocation("");
    pacsDevice.setStoreServiceEnabled(false);
    pacsDevice.setStoreServicePort(0);

    return pacsDevice;
}

}
