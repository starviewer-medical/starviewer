/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
