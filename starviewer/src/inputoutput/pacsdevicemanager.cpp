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

#include "pacsdevicemanager.h"

#include "inputoutputsettings.h"
#include "logging.h"
#include "pacsdevice.h"

namespace udg {

namespace {

// Returns true if the given PACS is already configured and false otherwise.
bool isPacsConfigured(const PacsDevice &pacs)
{
    QList<PacsDevice> pacsList = PacsDeviceManager::getPacsList();

    foreach (PacsDevice pacsDevice, pacsList)
    {
        if (pacsDevice.isSamePacsDevice(pacs))
        {
            return true;
        }
    }

    return false;
}

// Converts a PacsDevice instance to a Settings::SettingsListItemType instance.
Settings::SettingsListItemType pacsDeviceToSettingsListItem(const PacsDevice &pacsDevice)
{
    Settings::SettingsListItemType item;
    item["ID"] = pacsDevice.getID();

    switch (pacsDevice.getType())
    {
        case PacsDevice::Type::Dimse:
            item["Type"] = "DIMSE";
            item["AETitle"] = pacsDevice.getAETitle();
            item["PacsHostname"] = pacsDevice.getAddress();
            item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
            item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
            item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
            item["StoreServicePort"] = pacsDevice.getStoreServicePort();
            break;

        case PacsDevice::Type::Wado:
            item["Type"] = "WADO";
            item["BaseUri"] = pacsDevice.getBaseUri();
            break;

        case PacsDevice::Type::WadoUriDimse:
            item["Type"] = "WADO-URI+DIMSE";
            item["AETitle"] = pacsDevice.getAETitle();
            item["PacsHostname"] = pacsDevice.getAddress();
            item["QueryRetrieveServiceEnabled"] = pacsDevice.isQueryRetrieveServiceEnabled();
            item["PacsPort"] = pacsDevice.getQueryRetrieveServicePort();
            item["StoreServiceEnabled"] = pacsDevice.isStoreServiceEnabled();
            item["StoreServicePort"] = pacsDevice.getStoreServicePort();
            item["BaseUri"] = pacsDevice.getBaseUri();
    }

    item["Institution"] = pacsDevice.getInstitution();
    item["Location"] = pacsDevice.getLocation();
    item["Description"] = pacsDevice.getDescription();

    return item;
}

// Converts a Settings::SettingsListItemType instance to a PacsDevice instance.
PacsDevice settingsListItemToPacsDevice(const Settings::SettingsListItemType &item)
{
    PacsDevice pacsDevice;
    pacsDevice.setID(item["ID"].toString());

    if (!item.contains("Type") || item["Type"].toString() == "DIMSE")
    {
        pacsDevice.setType(PacsDevice::Type::Dimse);
        pacsDevice.setAETitle(item["AETitle"].toString());
        pacsDevice.setAddress(item["PacsHostname"].toString());
        pacsDevice.setQueryRetrieveServiceEnabled(item.value("QueryRetrieveServiceEnabled", true).toBool());
        pacsDevice.setQueryRetrieveServicePort(item["PacsPort"].toInt());

        if (!item.contains("StoreServiceEnabled"))
        {
            pacsDevice.setStoreServiceEnabled(true);
            pacsDevice.setStoreServicePort(item["PacsPort"].toInt());
        }
        else
        {
            pacsDevice.setStoreServiceEnabled(item["StoreServiceEnabled"].toBool());
            pacsDevice.setStoreServicePort(item["StoreServicePort"].toInt());
        }
    }
    else if (item["Type"].toString() == "WADO")
    {
        pacsDevice.setType(PacsDevice::Type::Wado);
        pacsDevice.setBaseUri(item["BaseUri"].toUrl());
    }
    else if (item["Type"].toString() == "WADO-URI+DIMSE")
    {
        pacsDevice.setType(PacsDevice::Type::WadoUriDimse);
        pacsDevice.setAETitle(item["AETitle"].toString());
        pacsDevice.setAddress(item["PacsHostname"].toString());
        pacsDevice.setQueryRetrieveServiceEnabled(item.value("QueryRetrieveServiceEnabled", true).toBool());
        pacsDevice.setQueryRetrieveServicePort(item["PacsPort"].toInt());

        if (!item.contains("StoreServiceEnabled"))
        {
            pacsDevice.setStoreServiceEnabled(true);
            pacsDevice.setStoreServicePort(item["PacsPort"].toInt());
        }
        else
        {
            pacsDevice.setStoreServiceEnabled(item["StoreServiceEnabled"].toBool());
            pacsDevice.setStoreServicePort(item["StoreServicePort"].toInt());
        }

        pacsDevice.setBaseUri(item["BaseUri"].toUrl());
    }

    pacsDevice.setInstitution(item["Institution"].toString());
    pacsDevice.setLocation(item["Location"].toString());
    pacsDevice.setDescription(item["Description"].toString());

    return pacsDevice;
}

}

bool PacsDeviceManager::addPacs(PacsDevice &pacs)
{
    if (isPacsConfigured(pacs))
    {
        return false;   // PACS already exists
    }

    QList<PacsDevice> pacsList = getPacsList();

    // Assign ID. It will be 0 if there is no other PACS and the highest ID + 1 otherwise.
    int newId = pacsList.size();    // initialize directly to the size; it will probably be the final id

    for (const PacsDevice &pacs : pacsList)
    {
        int id = pacs.getID().toInt();

        if (id >= newId)
        {
            newId = id + 1;
        }
    }

    pacs.setID(QString::number(newId));

    Settings().addListItem(InputOutputSettings::PacsListConfigurationSectionName, pacsDeviceToSettingsListItem(pacs));

    return true;
}

void PacsDeviceManager::updatePacs(PacsDevice &pacsToUpdate)
{
    PacsDevice oldPacs = getPacsDeviceById(pacsToUpdate.getID());

    if (oldPacs.isEmpty())
    {
        return; // PACS does not exist
    }

    QList<PacsDevice> pacsList = getPacsList();

    // Delete all PACS and then add them again with one of them updated
    Settings().remove(InputOutputSettings::PacsListConfigurationSectionName);

    for (PacsDevice &device : pacsList)
    {
        if (pacsToUpdate.getID() == device.getID())
        {
            addPacs(pacsToUpdate);
        }
        else
        {
            addPacs(device);
        }
    }
}

void PacsDeviceManager::deletePacs(const QString &pacsID)
{
    PacsDevice pacsToDelete = getPacsDeviceById(pacsID);

    if (pacsToDelete.isEmpty())
    {
        return; // PACS does not exist
    }

    QList<PacsDevice> pacsList = getPacsList();

    // Delete all PACS and then add them again except for the deleted one
    Settings().remove(InputOutputSettings::PacsListConfigurationSectionName);

    for (PacsDevice &device : pacsList)
    {
        if (pacsID != device.getID())
        {
            addPacs(device);
        }
    }
}

QList<PacsDevice> PacsDeviceManager::getPacsList(PacsFilter filter)
{
    Settings::SettingListType list = Settings().getList(InputOutputSettings::PacsListConfigurationSectionName);
    QList<PacsDevice> configuredPacsList;

    for (const Settings::SettingsListItemType &item : list)
    {
        PacsDevice pacs = settingsListItemToPacsDevice(item);
        bool include = filter.testFlag(CanRetrieve) && (
                    ((pacs.getType() == PacsDevice::Type::Dimse || pacs.getType() == PacsDevice::Type::WadoUriDimse) && pacs.isQueryRetrieveServiceEnabled())
                    || pacs.getType() == PacsDevice::Type::Wado
                    );
        include |= filter.testFlag(CanStore) && (
                    ((pacs.getType() == PacsDevice::Type::Dimse || pacs.getType() == PacsDevice::Type::WadoUriDimse) && pacs.isStoreServiceEnabled())
                    || pacs.getType() == PacsDevice::Type::Wado
                    );
        include &= filter.testFlag(OnlyDefault) ? pacs.isDefault() : true;

        if (include)
        {
            configuredPacsList.append(pacs);
        }
    }

    return configuredPacsList;
}

PacsDevice PacsDeviceManager::getPacsDeviceById(const QString &pacsID)
{
    QList<PacsDevice> pacsList = getPacsList();

    for (const PacsDevice &pacs : pacsList)
    {
        if (pacs.getID() == pacsID)
        {
            return pacs;
        }
    }

    WARN_LOG(QString("PACS with ID %1 not found.").arg(pacsID));

    return PacsDevice();
}

QList<PacsDevice> PacsDeviceManager::removeDuplicatePacsFromList(const QList<PacsDevice> &pacsDeviceList)
{
    QList<PacsDevice> pacsDeviceListWithoutDuplicates;

    for (const PacsDevice &pacsDevice : pacsDeviceList)
    {
        if (!isAddedSamePacsDeviceInList(pacsDeviceListWithoutDuplicates, pacsDevice))
        {
            pacsDeviceListWithoutDuplicates.append(pacsDevice);
        }
    }

    return pacsDeviceListWithoutDuplicates;
}

bool PacsDeviceManager::isAddedSamePacsDeviceInList(const QList<PacsDevice> &pacsDeviceList, const PacsDevice &pacsDevice)
{
    for (const PacsDevice &pacsDeviceInList : pacsDeviceList)
    {
        if (pacsDeviceInList.isSamePacsDevice(pacsDevice))
        {
            return true;
        }
    }

    return false;
}

}
