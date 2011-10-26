#include "dicomsource.h"

namespace udg {

void DICOMSource::addRetrievePACS(const PacsDevice &pacsDeviceToAdd)
{
    if (!isAddedSamePacsDevice(pacsDeviceToAdd))
    {
        m_retrievePACS.append(pacsDeviceToAdd);
    }
}

void DICOMSource::removeRetrievePACS(const PacsDevice &pacsDeviceToRemove)
{
    foreach (PacsDevice pacsDevice, m_retrievePACS)
    {
        if (pacsDevice.isSamePacsDevice(pacsDeviceToRemove))
        {
            m_retrievePACS.removeOne(pacsDevice);

        }
    }
}

QList<PacsDevice> DICOMSource::getRetrievePACS() const
{
    return m_retrievePACS;
}

void DICOMSource::addPACSDeviceFromDICOMSource(const DICOMSource &DICOMSourceToAdd)
{
    foreach(PacsDevice pacsDevice, DICOMSourceToAdd.getRetrievePACS())
    {
        this->addRetrievePACS(pacsDevice);
    }
}

bool DICOMSource::operator==(const DICOMSource &DICOMSourceToAdd)
{
    if (DICOMSourceToAdd.getRetrievePACS().count() != this->getRetrievePACS().count())
    {
        return false;
    }

    foreach(PacsDevice pacsDevice, DICOMSourceToAdd.getRetrievePACS())
    {
        if (!isAddedSamePacsDevice(pacsDevice))
        {
            return false;
        }
    }

    return true;
}

bool DICOMSource::isAddedSamePacsDevice(const PacsDevice pacsDevice) const
{
    foreach (PacsDevice pacsDeviceInList, m_retrievePACS)
    {
        if (pacsDeviceInList.isSamePacsDevice(pacsDevice))
        {
            return true;
        }
    }

    return false;
}

}
