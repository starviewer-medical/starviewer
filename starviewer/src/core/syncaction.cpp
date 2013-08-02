#include "syncaction.h"

namespace udg {

SyncAction::SyncAction()
{
}

SyncAction::~SyncAction()
{
    foreach (SyncCriterion *criterion, m_defaultSyncCriteria)
    {
        delete criterion;
    }
}

SyncActionMetaData SyncAction::getMetaData()
{
    if (m_metaData.isEmpty())
    {
        setupMetaData();
    }

    return m_metaData;
}

QList<SyncCriterion*> SyncAction::getDefaultSyncCriteria()
{
    if (m_defaultSyncCriteria.isEmpty())
    {
        setupDefaultSyncCriteria();
    }

    return m_defaultSyncCriteria;
}

} // End namespace udg
