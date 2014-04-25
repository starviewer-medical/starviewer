#include "syncactionsconfiguration.h"

#include "syncaction.h"

namespace udg {

SyncActionsConfiguration::SyncActionsConfiguration()
{
}

SyncActionsConfiguration::~SyncActionsConfiguration()
{
    cleanupConfiguredSyncCriteria();
}

void SyncActionsConfiguration::enableSyncAction(const SyncActionMetaData &syncActionMetaData, bool enable)
{
    if (syncActionMetaData.isEmpty())
    {
        return;
    }

    QString syncActionName = syncActionMetaData.getName();
    if (enable)
    {
        m_enabledSyncActions.insert(syncActionName, syncActionMetaData);
    }
    else
    {
        m_enabledSyncActions.remove(syncActionName);
    }
}

bool SyncActionsConfiguration::isSyncActionEnabled(const SyncActionMetaData &syncActionMetaData)
{
    return m_enabledSyncActions.contains(syncActionMetaData.getName());
}

QList<SyncCriterion*> SyncActionsConfiguration::getSyncCriteria(SyncAction *syncAction)
{
    if (!syncAction)
    {
        return QList<SyncCriterion*>();
    }

    QList<SyncCriterion*> criteria = m_configuredSyncCriteria.values(syncAction->getMetaData());
    if (criteria.isEmpty())
    {
        // If no criteria are configured, we take the default ones
        criteria = syncAction->getDefaultSyncCriteria();
    }

    return criteria;
}

void SyncActionsConfiguration::addSyncCriterion(const SyncActionMetaData &syncActionMetaData, SyncCriterion *criterion)
{
    m_configuredSyncCriteria.insert(syncActionMetaData, criterion);
}

void SyncActionsConfiguration::removeSyncCriteria(const SyncActionMetaData &syncActionMetaData)
{
    // Delete each SyncCriterion pointer before removing key and its values
    QMultiMap<SyncActionMetaData, SyncCriterion*>::iterator it = m_configuredSyncCriteria.find(syncActionMetaData);
    while (it != m_configuredSyncCriteria.end() && it.key() == syncActionMetaData)
    {
        delete it.value();
        ++it;
    }

    m_configuredSyncCriteria.remove(syncActionMetaData);
}

void SyncActionsConfiguration::cleanupConfiguredSyncCriteria()
{
    QMultiMap<SyncActionMetaData, SyncCriterion*>::iterator it = m_configuredSyncCriteria.begin();
    while (it != m_configuredSyncCriteria.end())
    {
        delete it.value();
        ++it;
    }
}

} // End namespace udg
