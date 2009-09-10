/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacsdevicemanager.h"

#include <QString>
#include "pacsdevice.h"
#include "logging.h"

namespace udg {

// TODO passar-ho inputoutputsettings?
const QString PacsDeviceManager::PacsListConfigurationSectionName = "PacsList";

PacsDeviceManager::PacsDeviceManager()
{
}

PacsDeviceManager::~PacsDeviceManager()
{
}

bool PacsDeviceManager::addPACS(const PacsDevice &pacs)
{
    bool ok = !this->isPACSConfigured(pacs);
    if( ok )
    {
        Settings settings;
        settings.addListItem( PacsListConfigurationSectionName, pacsDeviceToKeyValueMap(pacs) );
    }

    return ok;
}

void PacsDeviceManager::updatePACS(const PacsDevice &pacsToUpdate)
{
    Settings settings;
    settings.setListItem( pacsToUpdate.getID().toInt(),PacsListConfigurationSectionName, pacsDeviceToKeyValueMap(pacsToUpdate) );
}

bool PacsDeviceManager::deletePACS( const QString &pacsIDString)
{
    bool ok = false;
    int pacsID = pacsIDString.toInt(&ok);

    if(ok)
    {
        Settings settings;
        settings.removeListItem( PacsListConfigurationSectionName, pacsID );
    }

    return ok;
}

QList<PacsDevice> PacsDeviceManager::getPACSList( bool onlyDefault )
{
    QList<PacsDevice> configuredPacsList;
    Settings settings;
    Settings::SettingListType list = settings.getList(PacsListConfigurationSectionName);
    foreach( Settings::KeyValueMapType item, list )
    {
        PacsDevice pacs;
        pacs = keyValueMapToPacsDevice(item);
        // depenent del paràmetre "onlyDefault" afegirem o no els pacs
        if( (onlyDefault && pacs.isDefault()) || !onlyDefault )
        {
            configuredPacsList << pacs;
        }
    }

    return configuredPacsList;
}

PacsDevice PacsDeviceManager::getPACSDeviceByID( const QString &pacsIDString )
{
    QList<PacsDevice> pacsList = getPACSList();
    bool ok = false;
    int pacsID = pacsIDString.toInt(&ok);
    PacsDevice pacs;

    if (ok)
    {
        if (pacsID < pacsList.count()) 
        {
            pacs = pacsList.at(pacsID);
        }
        else 
            ERROR_LOG("No existeix cap PACS amb aquest ID: " + pacsIDString);
    }
    else 
        ERROR_LOG("No s'ha pogut convertir l'string amb l'id del PACS a enter, el valor de l'string és: " + pacsIDString);

    return pacs;
}

bool PacsDeviceManager::isPACSConfigured(const PacsDevice &pacs)
{
    QList<PacsDevice> pacsList = getPACSList();

    foreach(PacsDevice pacsDevice, pacsList)
    {
        if (pacsDevice.getAETitle() == pacs.getAETitle() && 
            pacsDevice.getPort() == pacs.getPort() &&
            pacsDevice.getAddress() == pacs.getAddress())
        {
            return true;
        }
    }

    return false;
}

Settings::KeyValueMapType PacsDeviceManager::pacsDeviceToKeyValueMap( const PacsDevice &pacsDevice )
{
    Settings::KeyValueMapType item;

    item["ID"] = pacsDevice.getID();
    item["AETitle"] = pacsDevice.getAETitle();
    item["PacsPort"] = pacsDevice.getPort();
    item["Location"] = pacsDevice.getLocation();
    item["Institution"] = pacsDevice.getInstitution();
    item["PacsHostname"] = pacsDevice.getAddress();
    item["Description"] = pacsDevice.getDescription();

    return item;
}

PacsDevice PacsDeviceManager::keyValueMapToPacsDevice( const Settings::KeyValueMapType &item )
{
    PacsDevice pacsDevice;
    // TODO cal comprovar que hi ha les claus que volem? sinó quedarà amb valors empty
    pacsDevice.setID( item.value("ID").toString() );
    pacsDevice.setAETitle( item.value("AETitle" ).toString() );
    pacsDevice.setPort( item.value("PacsPort" ).toString() );
    pacsDevice.setLocation( item.value("Location" ).toString() );
    pacsDevice.setInstitution( item.value("Institution" ).toString() );
    pacsDevice.setAddress( item.value("PacsHostname" ).toString() );
    pacsDevice.setDescription( item.value("Description" ).toString() );

    return pacsDevice;
}

};
