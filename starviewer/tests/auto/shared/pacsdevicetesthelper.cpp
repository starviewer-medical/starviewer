#include "pacsdevicetesthelper.h"

using namespace udg;

namespace testing {

PacsDevice PACSDeviceTestHelper::createPACSDeviceByID(QString ID)
{
    PacsDevice pacsDevice;

    pacsDevice.setID(ID);
    pacsDevice.setAddress(ID);
    pacsDevice.setAETitle(ID);
    pacsDevice.setQueryRetrieveServiceEnabled(true);
    pacsDevice.setQueryRetrieveServicePort(4006);
    pacsDevice.setInstitution(ID);
    pacsDevice.setDescription("");
    pacsDevice.setLocation("");
    pacsDevice.setStoreServiceEnabled(false);
    pacsDevice.setStoreServicePort(0);

    return pacsDevice;
}

PacsDevice PACSDeviceTestHelper::createDimsePacsDevice(QString ID, QString AETitle, QString address, int queryPort)
{
    PacsDevice pacsDevice = createPACSDeviceByID(ID);

    pacsDevice.setAETitle(AETitle);
    pacsDevice.setAddress(address);
    pacsDevice.setQueryRetrieveServicePort(queryPort);

    return pacsDevice;
}

PacsDevice PACSDeviceTestHelper::createWadoPacsDevice(QString id, const QString &baseUri)
{
    PacsDevice pacsDevice;
    pacsDevice.setID(std::move(id));
    pacsDevice.setType(PacsDevice::Type::Wado);
    pacsDevice.setBaseUri(baseUri);
    return pacsDevice;
}

PacsDevice PACSDeviceTestHelper::createWadoUriDimsePacsDevice(QString id, QString aeTitle, QString address, int queryPort, const QString &baseUri)
{
    PacsDevice pacsDevice = createPACSDeviceByID(std::move(id));
    pacsDevice.setType(PacsDevice::Type::WadoUriDimse);
    pacsDevice.setAETitle(std::move(aeTitle));
    pacsDevice.setAddress(std::move(address));
    pacsDevice.setQueryRetrieveServicePort(queryPort);
    pacsDevice.setBaseUri(baseUri);
    return pacsDevice;
}

}
