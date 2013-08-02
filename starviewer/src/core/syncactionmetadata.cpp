#include "syncactionmetadata.h"

namespace udg {

SyncActionMetaData::SyncActionMetaData()
{
}

SyncActionMetaData::SyncActionMetaData(const QString &name, const QString &interfaceName, const QString &settingsName)
{
    m_name = name;
    m_userInterfaceName = interfaceName;
    m_settingsName = settingsName;
}

SyncActionMetaData::~SyncActionMetaData()
{
}

QString SyncActionMetaData::getName() const
{
    return m_name;
}

QString SyncActionMetaData::getUserInterfaceName() const
{
    return m_userInterfaceName;
}

QString SyncActionMetaData::getSettingsName() const
{
    return m_settingsName;
}

bool SyncActionMetaData::isEmpty() const
{
    return m_name.isEmpty() && m_userInterfaceName.isEmpty() && m_settingsName.isEmpty();
}

bool SyncActionMetaData::operator <(const SyncActionMetaData &syncActionMetaData) const
{
    return m_name < syncActionMetaData.m_name;
}

bool SyncActionMetaData::operator ==(const SyncActionMetaData &syncActionMetaData) const
{
    return m_name == syncActionMetaData.m_name &&
        m_userInterfaceName == syncActionMetaData.m_userInterfaceName &&
        m_settingsName == syncActionMetaData.m_settingsName;
}

} // End namespace udg
