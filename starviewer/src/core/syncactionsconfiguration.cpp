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

#include "syncactionsconfiguration.h"

#include "syncaction.h"
#include "synccriterion.h"

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

QList<SyncActionMetaData> SyncActionsConfiguration::getEnabledSyncActions() const
{
    return m_enabledSyncActions.values();
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
