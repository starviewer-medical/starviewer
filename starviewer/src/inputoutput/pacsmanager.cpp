/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacsmanager.h"

#include <QString>
#include "pacsparameters.h"
#include "logging.h"

namespace udg {

// TODO passar-ho inputoutputsettings?
const QString PacsManager::PacsListConfigurationSectionName = "PacsList/";

PacsManager::PacsManager()
{
}

PacsManager::~PacsManager()
{
}

bool PacsManager::insertPacs(const PacsParameters &pacs)
{
    bool ok = !this->existPacs(pacs);
    if( ok )
    {
        Settings settings;
        settings.addListItem( PacsListConfigurationSectionName, pacsParametersToKeyValueMap(pacs) );
    }

    return ok;
}

void PacsManager::updatePacs(const PacsParameters &pacsToUpdate)
{
    Settings settings;
    settings.setListItem( pacsToUpdate.getPacsID().toInt(),PacsListConfigurationSectionName, pacsParametersToKeyValueMap(pacsToUpdate) );
}

QList<PacsParameters> PacsManager::queryPacsList()
{
    // els tornem tots
    return getConfiguredPacsList();
}

QList<PacsParameters> PacsManager::queryDefaultPacs()
{
    // només tornem els que estan per defecte
    return getConfiguredPacsList(true);
}

PacsParameters PacsManager::queryPacs( const QString &pacsIDString )
{
    QList<PacsParameters> pacsList = getConfiguredPacsList();
    bool ok = false;
    int pacsID = pacsIDString.toInt(&ok);
    PacsParameters pacs;

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

bool PacsManager::existPacs(const PacsParameters &pacs)
{
    QList<PacsParameters> pacsList = getConfiguredPacsList();

    foreach(PacsParameters pacsParameters, pacsList)
    {
        if (pacsParameters.getAEPacs() == pacs.getAEPacs() && 
            pacsParameters.getPacsPort() == pacs.getPacsPort() &&
            pacsParameters.getPacsAddress() == pacs.getPacsAddress())
        {
            return true;
        }
    }

    return false;
}

bool PacsManager::deletePacs( const QString &pacsIDString)
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

QList<PacsParameters> PacsManager::getConfiguredPacsList( bool onlyDefault )
{
    QList<PacsParameters> configuredPacsList;
    Settings settings;
    Settings::SettingListType list = settings.getList(PacsListConfigurationSectionName);
    foreach( Settings::KeyValueMapType item, list )
    {
        PacsParameters pacs;
        pacs = keyValueMapToPacsParameters(item);
        // depenent del paràmetre "onlyDefault" afegirem o no els pacs
        if( (onlyDefault && pacs.isDefault()) || !onlyDefault )
        {
            configuredPacsList << pacs;
        }
    }

    return configuredPacsList;
}

Settings::KeyValueMapType PacsManager::pacsParametersToKeyValueMap( const PacsParameters &parameters )
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

PacsParameters PacsManager::keyValueMapToPacsParameters( const Settings::KeyValueMapType &item )
{
    PacsParameters parameters;
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
