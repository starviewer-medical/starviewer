#include "settingsregistry.h"

namespace udg {

SettingsRegistry::SettingsRegistry()
{
}

SettingsRegistry::~SettingsRegistry()
{
}

void SettingsRegistry::addSetting( const QString &key, const QVariant &defaultValue )
{
    m_keyDefaultValueMap.insert( key, defaultValue );
}

QVariant SettingsRegistry::getDefaultValue( const QString &key )
{
    return m_keyDefaultValueMap.value(key);
}

} // end namespace udg
