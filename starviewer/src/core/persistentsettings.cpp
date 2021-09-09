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

#include "persistentsettings.h"

#include "logging.h"
#include "settingsparser.h"
#include "settingsregistry.h"
#include "starviewerapplication.h"

#include <QSettings>

namespace udg {

namespace {

// Fills and returns a settings list item (a map) from the given QSettings and the given key list.
Settings::SettingsListItemType fillSettingsListItemFromKeysList(const QStringList &keysList, QSettings *qsettings)
{
    Q_ASSERT(qsettings);

    Settings::SettingsListItemType item;

    foreach (const QString &key, keysList)
    {
        item[key] = qsettings->value(key);
    }

    return item;
}

// Fills the given QSettings with the keys and values from the given settings list item (a map).
void dumpSettingsListItem(const Settings::SettingsListItemType &item, QSettings *qsettings)
{
    Q_ASSERT(qsettings);

    QStringList keysList = item.keys();

    foreach (const QString &key, keysList)
    {
        qsettings->setValue(key, item.value(key));
    }
}

}

PersistentSettings::PersistentSettings()
{
    QSettings *userSettings = new QSettings(QSettings::UserScope, OrganizationNameString, ApplicationNameString);
    QSettings *systemSettings = new QSettings(QSettings::SystemScope, OrganizationNameString, ApplicationNameString);

    m_qsettingsObjectsMap.insert(Settings::UserLevel, userSettings);
    m_qsettingsObjectsMap.insert(Settings::SystemLevel, systemSettings);
}

PersistentSettings::~PersistentSettings()
{
    foreach (QSettings *setting, m_qsettingsObjectsMap)
    {
        delete setting;
    }
}

QVariant PersistentSettings::getValue(const QString &key) const
{
    // First get the value from settings. If it's empty get the default value from the settings registry.
    QVariant value = getSettingsObject(key)->value(key);

    if (value == QVariant())
    {
        value = SettingsRegistry::instance()->getDefaultValue(key);
    }

    // Parse the settings value if applicable
    Settings::Properties properties = SettingsRegistry::instance()->getProperties(key);

    if (properties.testFlag(Settings::Parseable))
    {
        value = SettingsParser::instance()->parse(value.toString());
    }

    return value;
}

void PersistentSettings::setValue(const QString &key, const QVariant &value)
{
    getSettingsObject(key)->setValue(key, value);
}


bool PersistentSettings::contains(const QString &key) const
{
    return getSettingsObject(key)->contains(key);
}

void PersistentSettings::remove(const QString &key)
{
    getSettingsObject(key)->remove(key);
}

Settings::SettingListType PersistentSettings::getList(const QString &key)
{
    Settings::SettingListType list;
    QSettings *qsettings = getSettingsObject(key);
    int size = qsettings->beginReadArray(key);

    for (int i = 0; i < size; ++i)
    {
        qsettings->setArrayIndex(i);
        // Each list item is a map with several keys and values
        list.append(fillSettingsListItemFromKeysList(qsettings->allKeys(), qsettings));
    }

    qsettings->endArray();

    return list;
}

void PersistentSettings::setList(const QString &key, const Settings::SettingListType &list)
{
    // Clear the previous value of the key
    remove(key);

    QSettings *qsettings = getSettingsObject(key);
    qsettings->beginWriteArray(key);

    for (int i = 0; i < list.size(); i++)
    {
        qsettings->setArrayIndex(i);
        dumpSettingsListItem(list[i], qsettings);
    }

    qsettings->endArray();
}

void PersistentSettings::addListItem(const QString &key, const Settings::SettingsListItemType &item)
{
    QSettings *qsettings = getSettingsObject(key);

    int arraySize = qsettings->beginReadArray(key);
    qsettings->endArray();

    qsettings->beginWriteArray(key);
    qsettings->setArrayIndex(arraySize);
    dumpSettingsListItem(item, qsettings);
    qsettings->endArray();
}

void PersistentSettings::setListItem(int index, const QString &key, const Settings::SettingsListItemType &item)
{
    QSettings *qsettings = getSettingsObject(key);

    int arraySize = qsettings->beginReadArray(key);
    qsettings->endArray();

    if (index >= 0 && index < arraySize)
    {
        qsettings->beginWriteArray(key, arraySize);
        qsettings->setArrayIndex(index);
        dumpSettingsListItem(item, qsettings);
        qsettings->endArray();
    }
    else
    {
        WARN_LOG("Trying to set a setting list item out of range.");
    }
}

void PersistentSettings::removeListItem(const QString &key, int index)
{
    Settings::SettingListType list = getList(key);

    if (index >= 0 && index < list.size())
    {
        list.removeAt(index);
        setList(key, list);
    }
    else
    {
        WARN_LOG("Trying to remove a setting list item out of range.");
    }
}

QSettings* PersistentSettings::getSettingsObject(const QString &key) const
{
    return m_qsettingsObjectsMap.value(SettingsRegistry::instance()->getAccessLevel(key));
}

} // namespace udg
