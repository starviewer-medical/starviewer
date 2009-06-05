/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacsmanager.h"

#include <QString>
#include <QSettings>

#include "pacsparameters.h"
#include "logging.h"

namespace udg {

const QString PacsManager::PacsListConfigurationSectionName = "PacsList";

PacsManager::PacsManager()
{
}

PacsManager::~PacsManager()
{
}

bool PacsManager::insertPacs(const PacsParameters &pacs)
{
    if(!this->existPacs(pacs))
    {
        QList<PacsParameters> pacsList = getConfiguredPacsList();
        pacsList.append(pacs);

        this->saveConfiguredPacsListToDisk(pacsList);

        return true;
    }
    return false;
}

void PacsManager::updatePacs(const PacsParameters &pacsToUpdate)
{
    QList<PacsParameters> pacsList = getConfiguredPacsList();

    pacsList[pacsToUpdate.getPacsID().toInt()] = pacsToUpdate;

    this->saveConfiguredPacsListToDisk(pacsList);
}

QList<PacsParameters> PacsManager::queryPacsList()
{
    return getConfiguredPacsList();
}

QList<PacsParameters> PacsManager::queryDefaultPacs()
{
    QList<PacsParameters> defaultPacs;
    QSettings settings;
    int size = settings.beginReadArray(PacsListConfigurationSectionName);

    for(int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        PacsParameters pacs = fillPacs(settings);
        if (pacs.getIsDefault()) defaultPacs.append(pacs);
    }
    settings.endArray();

    return defaultPacs;
}

PacsParameters PacsManager::queryPacs(QString pacsIDString)
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
        else ERROR_LOG("No existeix cap PACS amb aquest ID: " + pacsIDString);
    }
    else ERROR_LOG("No s'ha pogut convertir l'string amb l'id del PACS a enter, el valor de l'string és: " + pacsIDString);

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

bool PacsManager::deletePacs(QString pacsIDString)
{
    QList<PacsParameters> pacsList = getConfiguredPacsList();
    bool ok = false;
    int pacsID = pacsIDString.toInt(&ok);

    if (!ok) return false;

    pacsList.removeAt(pacsID);
    this->saveConfiguredPacsListToDisk(pacsList);

    return true;
}

QList<PacsParameters> PacsManager::getConfiguredPacsList()
{
    QList<PacsParameters> configuredPacsList;
    QSettings settings;
    int size = settings.beginReadArray(PacsListConfigurationSectionName);

    for(int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        PacsParameters pacs = fillPacs(settings);
        configuredPacsList.append(pacs);
    }
    settings.endArray();

    return configuredPacsList;
}

void PacsManager::saveConfiguredPacsListToDisk(const QList<PacsParameters> &pacsList)
{
    QSettings settings;

    settings.beginWriteArray(PacsListConfigurationSectionName);
    settings.remove(""); //Esborrem la llista de pacs abans de guardar la nova.
    for(int i = 0; i < pacsList.size(); ++i)
    {
        settings.setArrayIndex(i);

        settings.setValue("ID", i);
        settings.setValue("AETitle", pacsList.at(i).getAEPacs() );
        settings.setValue("PacsPort", pacsList.at(i).getPacsPort() );
        settings.setValue("Location", pacsList.at(i).getLocation() );
        settings.setValue("Institution", pacsList.at(i).getInstitution() );
        settings.setValue("Default", pacsList.at(i).getIsDefault() ? "S" : "N" );//Guardem el camp en format string
        settings.setValue("PacsHostname", pacsList.at(i).getPacsAddress() );
        settings.setValue("Description", pacsList.at(i).getDescription() );
    }
    settings.endArray();
}

PacsParameters PacsManager::fillPacs(const QSettings &settings)
{
    PacsParameters pacsParameters;

    pacsParameters.setPacsID( settings.value("ID" ).toString() );
    pacsParameters.setAEPacs( settings.value("AETitle" ).toString() );
    pacsParameters.setPacsPort( settings.value("PacsPort" ).toString() );
    pacsParameters.setLocation( settings.value("Location" ).toString() );
    pacsParameters.setInstitution( settings.value("Institution" ).toString() );
    pacsParameters.setIsDefault( settings.value("Default" ).toString() == "S" );
    pacsParameters.setPacsAddress( settings.value("PacsHostname" ).toString() );
    pacsParameters.setDescription( settings.value("Description" ).toString() );

    return pacsParameters;
}

};
