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
        StudyLayoutConfigSettingsConverter settingsConverter;
        Settings settings;
        settings.addListItem(CoreSettings::StudyLayoutConfigList, settingsConverter.toSettingsListItem(config));
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
    // Eliminem totess les configuracions que tinguem guardades a settings
    Settings settings;
    settings.remove(CoreSettings::StudyLayoutConfigList);

    // Recorrem tota la llista i les afegim de nou
    // Si trobem el que volem fer update, afegim l'actualitzat
    foreach (const StudyLayoutConfig &currentConfig, configList)
    {
        if (config.getModality() == currentConfig.getModality())
        {
            addItem(config);
            updated = true;
        }
        else
        {
            addItem(currentConfig);
        }
    }

    return updated;
}

bool StudyLayoutConfigSettingsManager::deleteItem(const StudyLayoutConfig &config)
{
    bool deleted = false;
    // Obtenim la llista completa de PACS
    QList<StudyLayoutConfig> configList = getConfigList();
    // Eliminem tots els PACS que tinguem guardats a disc
    Settings settings;
    settings.remove(CoreSettings::StudyLayoutConfigList);

    // Recorrem tota la llista de configuracions i les afegim de nou
    // excepte la que volem esborrar
    foreach (const StudyLayoutConfig &currentConfig, configList)
    {
        if (config.getModality() != currentConfig.getModality())
        {
            addItem(currentConfig);
        }
        else
        {
            deleted = true;
        }
    }
    
    return deleted;
}

}
