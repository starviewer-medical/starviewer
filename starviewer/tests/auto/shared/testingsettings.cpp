#include "testingsettings.h"

#include "coresettings.h"
#include "inputoutputsettings.h"
#include "interfacesettings.h"
#include "mprsettings.h"
#include "settingsregistry.h"
#include "q2dviewersettings.h"
#include "q3dviewerextensionsettings.h"

namespace testing {

TestingSettings::TestingSettings()
{
    CoreSettings().init();
    InputOutputSettings().init();
    InterfaceSettings().init();
    MPRSettings().init();
    Q2DViewerSettings().init();
    Q3DViewerExtensionSettings().init();
}

QVariant TestingSettings::getValue(const QString &key) const
{
    if (this->contains(key))
    {
        return m_settings[key];
    }
    else
    {
        return SettingsRegistry::instance()->getDefaultValue(key);
    }
}

void TestingSettings::setValue(const QString &key, const QVariant &value)
{
    m_settings[key] = value;
}

bool TestingSettings::contains(const QString &key) const
{
    return m_settings.contains(key);
}

void TestingSettings::remove(const QString &key)
{
    m_settings.remove(key);
}

Settings::SettingListType TestingSettings::getList(const QString &key)
{
    QList<QVariant> variantsList = m_settings.value(key).toList();  // empty list if the key does not exist or does not contain a list
    Settings::SettingListType list;

    for (const QVariant &item : variantsList)
    {
        list.append(item.toMap());
    }

    return list;
}

void TestingSettings::setList(const QString &key, const Settings::SettingListType &list)
{
    QList<QVariant> variantsList;

    for (const Settings::SettingsListItemType &item : list)
    {
        variantsList.append(item);
    }

    m_settings[key] = variantsList;
}

void TestingSettings::addListItem(const QString &key, const Settings::SettingsListItemType &item)
{
    auto list = getList(key);
    list.append(item);
    setList(key, list);
}

void TestingSettings::setListItem(int index, const QString &key, const Settings::SettingsListItemType &item)
{
    auto list = getList(key);
    list[index] = item;
    setList(key, list);
}

void TestingSettings::removeListItem(const QString &key, int index)
{
    auto list = getList(key);
    list.removeAt(index);
    setList(key, list);
}

bool TestingSettings::operator==(const TestingSettings &that) const
{
    return this->m_settings == that.m_settings;
}

}
