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

#include "studylayoutconfigsettingsmanager.h"

#include "studylayoutconfigsettingsconverter.h"
#include "studylayoutconfig.h"
#include "coresettings.h"
#include "logging.h"

namespace udg {

StudyLayoutConfigSettingsManager::StudyLayoutConfigSettingsManager()
{
}

StudyLayoutConfigSettingsManager::~StudyLayoutConfigSettingsManager()
{
}

QList<StudyLayoutConfig> StudyLayoutConfigSettingsManager::getConfigList() const
{
    StudyLayoutConfigSettingsConverter settingsConverter;
    QList<StudyLayoutConfig> configList;
    Settings settings;
    Settings::SettingListType list = settings.getList(CoreSettings::StudyLayoutConfigList);
    foreach (Settings::SettingsListItemType item, list)
    {
        StudyLayoutConfig config;
        config = settingsConverter.fromSettingsListItem(item);
        configList << config;
    }

    return configList;
}

void StudyLayoutConfigSettingsManager::setConfigList(const QList<StudyLayoutConfig> &configList)
{
    Settings::SettingListType settingsList;
    settingsList.reserve(configList.size());
    StudyLayoutConfigSettingsConverter settingsConverter;

    for (const StudyLayoutConfig &config : configList)
    {
        settingsList.append(settingsConverter.toSettingsListItem(config));
    }

    Settings().setList(CoreSettings::StudyLayoutConfigList, settingsList);
}

bool StudyLayoutConfigSettingsManager::addItem(const StudyLayoutConfig &config)
{
    bool ok = true;
    QList<StudyLayoutConfig> configsList = getConfigList();
    foreach (const StudyLayoutConfig &item, configsList)
    {
        if (item.getModality() == config.getModality())
        {
            ok = false;
            break;
        }
    }

    if (ok)
    {
        configsList.append(config);
        setConfigList(configsList);
    }
    else
    {
        DEBUG_LOG("Ja existeix una configuració de layout automàtic per aquesta modalitat: " + config.getModality() + ". No s'afegirà.");
    }

    return ok;
}

bool StudyLayoutConfigSettingsManager::updateItem(const StudyLayoutConfig &config)
{
    bool updated = false;
    // Obtenim la llista completa de StudyLayoutConfigs
    QList<StudyLayoutConfig> configList = getConfigList();

    for (int i = 0; i < configList.size(); i++)
    {
        const StudyLayoutConfig &currentConfig = configList[i];

        if (config.getModality() == currentConfig.getModality())
        {
            configList[i] = config;
            updated = true;
            break;
        }
    }

    if (updated)
    {
        setConfigList(configList);
    }

    return updated;
}

bool StudyLayoutConfigSettingsManager::deleteItem(const StudyLayoutConfig &config)
{
    bool deleted = false;
    // Obtenim la llista completa de PACS
    QList<StudyLayoutConfig> configList = getConfigList();

    for (int i = 0; i < configList.size(); i++)
    {
        const StudyLayoutConfig &currentConfig = configList[i];

        if (config.getModality() == currentConfig.getModality())
        {
            configList.removeAt(i);
            deleted = true;
            break;
        }
    }

    if (deleted)
    {
        setConfigList(configList);
    }
    
    return deleted;
}

}
