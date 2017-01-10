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

#include "settingsregistry.h"

#include "logging.h"
#include "settingsaccesslevelfilereader.h"

#include <QApplication>
#include <QFile>

namespace udg {

SettingsRegistry::SettingsRegistry()
{
    loadAccesLevelTable();
}

SettingsRegistry::~SettingsRegistry()
{
}

void SettingsRegistry::addSetting(const QString &key, const QVariant &defaultValue, Settings::Properties properties)
{
    m_keyDefaultValueAndPropertiesMap.insert(key, qMakePair(defaultValue, properties));
}

QVariant SettingsRegistry::getDefaultValue(const QString &key)
{
    return m_keyDefaultValueAndPropertiesMap.value(key).first;
}

Settings::AccessLevel SettingsRegistry::getAccessLevel(const QString &key) const
{
    Settings::AccessLevel accessLevel = Settings::UserLevel;

    if (m_accessLevelTable.contains(key))
    {
        accessLevel = m_accessLevelTable.value(key);
    }

    return accessLevel;
}

Settings::Properties SettingsRegistry::getProperties(const QString &key)
{
    return m_keyDefaultValueAndPropertiesMap.value(key).second;
}

void SettingsRegistry::loadAccesLevelTable()
{
    // We can arrive here before creating QApplication due to the initial HDPI support, thus this check is needed
    if (!qApp)
    {
        return;
    }

    // Al directori on s'instal·la l'aplicació tindrem
    // un .ini que definirà els nivells d'accés de cada settings
    QString filePath = qApp->applicationDirPath() + "/settingsAccessLevel.ini";
    QFile file(filePath);
    if (!file.exists())
    {
        DEBUG_LOG("L'arxiu [" + filePath + "] No existeix. No es poden carregar els nivells d'accés");
    }
    else
    {
        DEBUG_LOG("Llegim arxiu d'Access Level de settings: [" + filePath + "]");
        SettingsAccessLevelFileReader fileReader;
        if (fileReader.read(filePath))
        {
            m_accessLevelTable = fileReader.getAccessLevelTable();
        }
    }
}

} // End namespace udg
