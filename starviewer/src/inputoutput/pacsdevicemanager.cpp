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

bool PacsDeviceManager::insertPacs(const PacsDevice &pacs)
{
    bool ok = !this->existPacs(pacs);
    if( ok )
    {
        Settings settings;
        settings.addListItem( PacsListConfigurationSectionName, pacsDeviceToKeyValueMap(pacs) );
    }

    return ok;
}

void PacsDeviceManager::updatePacs(const PacsDevice &pacsToUpdate)
{
    Settings settings;
    settings.setListItem( pacsToUpdate.getPacsID().toInt(),PacsListConfigurationSectionName, pacsDeviceToKeyValueMap(pacsToUpdate) );
}

QList<PacsDevice> PacsDeviceManager::queryPacsList()
{
    // els tornem tots
    return getConfiguredPacsList();
}

QList<PacsDevice> PacsDeviceManager::queryDefaultPacs()
{
    // només tornem els que estan per defecte
    return getConfiguredPacsList(true);
}

PacsDevice PacsDeviceManager::queryPacs( const QString &pacsIDString )
{
    QList<PacsDevice> pacsList = getConfiguredPacsList();
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

bool PacsDeviceManager::existPacs(const PacsDevice &pacs)
{
    QList<PacsDevice> pacsList = getConfiguredPacsList();

    foreach(PacsDevice pacsDevice, pacsList)
    {
        if (pacsDevice.getAEPacs() == pacs.getAEPacs() && 
            pacsDevice.getPacsPort() == pacs.getPacsPort() &&
            pacsDevice.getPacsAddress() == pacs.getPacsAddress())
        {
            return true;
        }
    }

    return false;
}

bool PacsDeviceManager::deletePacs( const QString &pacsIDString)
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

QList<PacsDevice> PacsDeviceManager::getConfiguredPacsList( bool onlyDefault )
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

Settings::KeyValueMapType PacsDeviceManager::pacsDeviceToKeyValueMap( const PacsDevice &parameters )
{
    Settings::KeyValueMapType item;
        
    item["ID"] = parameters.getPacsID();
    item["AETitle"] = parameters.getAEPacs();
    item["PacsPort"] = parameters.getPacsPort();
    item["Location"] = parameters.getLocation();
    item["Institution"] = parameters.getInstitution();
    item["Default"] = parameters.isDefault() ? "S" : "N";//Guardem el camp en format string
    item["PacsHostname"] = parameters.getPacsAddress();
    item["Description"] = parameters.getDescription();

    return item;
}

PacsDevice PacsDeviceManager::keyValueMapToPacsDevice( const Settings::KeyValueMapType &item )
{
    PacsDevice parameters;
    // TODO cal comprovar que hi ha les claus que volem? sinó quedarà amb valors empty
    parameters.setPacsID( item.value("ID").toString() );
    parameters.setAEPacs( item.value("AETitle" ).toString() );
    parameters.setPacsPort( item.value("PacsPort" ).toString() );
    parameters.setLocation( item.value("Location" ).toString() );
    parameters.setInstitution( item.value("Institution" ).toString() );
    
    // TODO la clau "Default" s'hauria de renombrar per una altre ja que en el cas del 
    // registre de windows pot donar problemes, ja que ve a ser una paraula "reservada"
    // Un nom més adient seria isDefault o isADefaultPACS, per exemple
    // cal fer aquest comprovació ja que depèn com s'hagin obtingut les dades per accedir a 
    // "Default", s'hauria de fer amb la clau "."
    QString isDefault;
    if( item.contains("Default") )
        isDefault = item.value("Default").toString();
    else
        isDefault = item.value(".").toString();
    parameters.setDefault( isDefault == "S" );
    
    parameters.setPacsAddress( item.value("PacsHostname" ).toString() );
    parameters.setDescription( item.value("Description" ).toString() );

    return parameters;
}

};
