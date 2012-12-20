#include "studylayoutconfigsloader.h"

#include "studylayoutconfig.h"
#include "studylayoutconfigsettingsmanager.h"

#include <QStringList>

namespace udg {

StudyLayoutConfigsLoader::StudyLayoutConfigsLoader()
{
}

StudyLayoutConfigsLoader::~StudyLayoutConfigsLoader()
{
}

void StudyLayoutConfigsLoader::load()
{
    // Llistat on mirem quines modalitats cal donar-li el valor per defecte en cas que no els tinguem ja configurats als settings
    QStringList modalitiesToLoadDefaultConfig;
    modalitiesToLoadDefaultConfig << "CR" << "CT" << "DX" << "ES" << "MG" << "MR" << "NM" << "OP" << "PT" << "RF" << "SC" << "US" << "XA" << "XC";

    StudyLayoutConfigSettingsManager settingsManager;
    QList<StudyLayoutConfig> configuredModalities = settingsManager.getConfigList();

    foreach (const StudyLayoutConfig &config, configuredModalities)
    {
        // Si tenim una modalitat configurada als settings, llavors no la carregarem dels valors per defecte
        modalitiesToLoadDefaultConfig.removeAll(config.getModality());
    }

    // Per cada modalitat que no estigui ja als settings, li carreguem els seus valors per defecte
    foreach (const QString &modality, modalitiesToLoadDefaultConfig)
    {
        settingsManager.addItem(getDefaultConfigForModality(modality));
    }
}

StudyLayoutConfig StudyLayoutConfigsLoader::getDefaultConfigForModality(const QString &modality)
{
    if (modality == "CR" || modality == "DX" || modality == "ES" || modality == "MG" || modality == "OP" || modality == "RF" || modality == "US" || modality == "XC")
    {
        // TODO De moment RF es posa en la categoria de desplegar per imatges per seguir com estan fets els HP, 
        // però cal veure si és millor fer que RF tingui un desplegament a nivell de sèries
        return StudyLayoutConfig(modality, StudyLayoutConfig::UnfoldImages, StudyLayoutConfig::LeftToRightFirst);
    }
    else if (modality == "CT")
    {
        StudyLayoutConfig config(modality, StudyLayoutConfig::UnfoldSeries, StudyLayoutConfig::LeftToRightFirst);
        config.addExclusionCriteria(StudyLayoutConfig::Localizer);
        return config;
    }
    else if (modality == "MR")
    {
        StudyLayoutConfig config(modality, StudyLayoutConfig::UnfoldSeries, StudyLayoutConfig::LeftToRightFirst);
        config.addExclusionCriteria(StudyLayoutConfig::Survey);
        return config;
    }
    else if (modality == "NM" || modality == "PT" || modality == "SC" || modality == "XA")
    {
        return StudyLayoutConfig(modality, StudyLayoutConfig::UnfoldSeries, StudyLayoutConfig::LeftToRightFirst);
    }
    else
    {
        return StudyLayoutConfig(modality);
    }
}

}
