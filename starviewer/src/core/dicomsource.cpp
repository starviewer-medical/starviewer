#include "dicomsource.h"

namespace udg {

void DICOMSource::addRetrievePACS(const PacsDevice &pacsDeviceToAdd)
{
    if (!m_retrievePACS.contains(pacsDeviceToAdd))
    {
        m_retrievePACS.append(pacsDeviceToAdd);
    }
}

void DICOMSource::removeRetrievePACS(const PacsDevice &pacsDeviceToRemove)
{
    m_retrievePACS.removeOne(pacsDeviceToRemove);
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
            if (!this->getRetrievePACS().contains(pacsDevice))
            {
                return false;
            }
    }

    return true;
}

};
