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
#include "inputoutputsettings.h"

namespace udg {

PacsDeviceManager::PacsDeviceManager()
{
}

PacsDeviceManager::~PacsDeviceManager()
{
}

bool PacsDeviceManager::addPACS(PacsDevice &pacs)
{
    // Si el PACS ja existeix no l'afegim
    bool ok = !this->isPACSConfigured(pacs);
    if( ok )
    {
        // En cas que existeixi, li assignarem l'ID
        QList<PacsDevice> pacsList = getPACSList();
        if( pacsList.isEmpty() )
        {
            // Si encara no teníem cap PACS, l'ID inicial serà 0
            pacs.setID( QString::number(0) );
        }
        else
        {
            // En cas que ja en tinguem de configurats, l'ID serà
            // l'ID més alt dels configurats + 1
            int highestID = 0;
            foreach( PacsDevice pacs, pacsList )
            {
                if( pacs.getID().toInt() > highestID )
                    highestID = pacs.getID().toInt();
            }
            pacs.setID( QString::number(highestID + 1) );
        }

        Settings settings;
        settings.addListItem( InputOutputSettings::PacsListConfigurationSectionName, pacsDeviceToKeyValueMap(pacs) );
    }

    return ok;
}

void PacsDeviceManager::updatePACS(PacsDevice &pacsToUpdate)
{
    // Obtenim la llista completa de PACS
    QList<PacsDevice> pacsList = getPACSList();
    // Eliminem tots els PACS que tinguem guardats a disc
    Settings settings;
    settings.remove(InputOutputSettings::PacsListConfigurationSectionName);

    // Recorrem tota la llista de PACS i els afegim de nou
    // Si trobem el que volem fer update, afegim l'actualitzat
    foreach( PacsDevice device, pacsList )
    {
        if( pacsToUpdate.getID() == device.getID() )
        {
            addPACS( pacsToUpdate );
        }
        else
        {
            addPACS( device );
        }
    }
}

bool PacsDeviceManager::deletePACS( const QString &pacsIDString)
{
    // Obtenim la llista completa de PACS
    QList<PacsDevice> pacsList = getPACSList();
    // Eliminem tots els PACS que tinguem guardats a disc
    Settings settings;
    settings.remove(InputOutputSettings::PacsListConfigurationSectionName);

    // Recorrem tota la llista de PACS i els afegim de nou
    // excepte el que volem esborrar
    foreach( PacsDevice device, pacsList )
    {
        if( pacsIDString != device.getID() )
        {
            addPACS( device );    
        }
    }
    return true;
}

QList<PacsDevice> PacsDeviceManager::getPACSList( bool onlyDefault )
{
    QList<PacsDevice> configuredPacsList;
    Settings settings;
    Settings::SettingListType list = settings.getList(InputOutputSettings::PacsListConfigurationSectionName);
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
    PacsDevice pacs;

    bool found = false;
    int i = 0;
    int count = pacsList.count();
    while( !found && i < count )
    {
        if( pacsIDString == pacsList.at(i).getID() )
        {
            found = true;
            pacs = pacsList.at(i);
        }
        i++;
    }

    if( !found )
    {
        DEBUG_LOG("No existeix cap PACS amb aquest ID: " + pacsIDString);
        ERROR_LOG("No existeix cap PACS amb aquest ID: " + pacsIDString);
    }
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
    item["PacsPort"] = QString().setNum(pacsDevice.getPort());
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
    pacsDevice.setPort( item.value("PacsPort" ).toString().toInt() );
    pacsDevice.setLocation( item.value("Location" ).toString() );
    pacsDevice.setInstitution( item.value("Institution" ).toString() );
    pacsDevice.setAddress( item.value("PacsHostname" ).toString() );
    pacsDevice.setDescription( item.value("Description" ).toString() );

    return pacsDevice;
}

};
