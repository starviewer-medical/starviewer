#include "syncactionmanager.h"

#include "signaltosyncactionmapperfactory.h"
#include "syncaction.h"
#include "synccriterion.h"
#include "syncactionsconfiguration.h"

#include "q2dviewer.h"

namespace udg {

SyncActionManager::SyncActionManager(QObject *parent)
 : QObject(parent)
{
    m_masterViewer = 0;
    m_enabled = false;
    
    setupSignalMappers();
    setupDefaultSyncActionsConfiguration();
}

SyncActionManager::~SyncActionManager()
{
}

void SyncActionManager::addSyncedViewer(QViewer *viewer)
{
    if (!m_syncedViewersSet.contains(viewer))
    {
        m_syncedViewersSet << viewer;
    }
}

void SyncActionManager::removeSyncedViewer(QViewer *viewer)
{
    if (viewer == m_masterViewer)
    {
        m_masterViewer = 0;
        updateMasterViewerMappers();
    }
    m_syncedViewersSet.remove(viewer);
}

void SyncActionManager::setMasterViewer(QViewer *viewer)
{
    if (m_syncedViewersSet.contains(viewer))
    {
        Q2DViewer *masterViewer2D = Q2DViewer::castFromQViewer(m_masterViewer);
        if (masterViewer2D)
        {
            disconnect(masterViewer2D, 0, this, 0);
        }

        m_masterViewer = viewer;
        updateMasterViewerMappers();

        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
        if (viewer2D)
        {
            connect(viewer2D, SIGNAL(restored()), this, SLOT(synchronize()));
            connect(viewer2D, SIGNAL(anatomicalViewChanged(AnatomicalPlane::AnatomicalPlaneType)), this, SLOT(synchronize()));
            connect(viewer2D, SIGNAL(newVolumesRendered()), this, SLOT(synchronize()));
        }

        if (m_masterViewer->getMainInput())
        {
            synchronize();
        }
    }
}

void SyncActionManager::clearSyncedViewersSet()
{
    m_syncedViewersSet.clear();
}

void SyncActionManager::setSyncActionsConfiguration(SyncActionsConfiguration *configuration)
{
    if (!configuration)
    {
        return;
    }

    m_syncActionsConfiguration = configuration;
}

void SyncActionManager::enable(bool enable)
{
    m_enabled = enable;

    if (m_enabled && m_masterViewer && m_masterViewer->getMainInput())
    {
        synchronize();
    }
}

void SyncActionManager::synchronize()
{
    foreach (SignalToSyncActionMapper *mapper, m_registeredSignalMappers)
    {
        mapper->mapProperty();
    }
}

void SyncActionManager::setupSignalMappers()
{
    // We create an instance of each of the registered signal mappers, add it to our own list, and then
    // we create the connection of each mapper with the corresponding slot to apply the SyncAction
    foreach (const SyncActionMetaData &syncActionMetaData, SignalToSyncActionMapperFactory::instance()->getFactoryIdentifiersList())
    {
        SignalToSyncActionMapper *mapper = SignalToSyncActionMapperFactory::instance()->create(syncActionMetaData);
        
        m_registeredSignalMappers << mapper;
        connect(mapper, SIGNAL(actionMapped(SyncAction*)), SLOT(applySyncAction(SyncAction*)));
    }
}

void SyncActionManager::setupDefaultSyncActionsConfiguration()
{
    m_syncActionsConfiguration = new SyncActionsConfiguration();

    foreach (const SyncActionMetaData &syncActionMetaData, SignalToSyncActionMapperFactory::instance()->getFactoryIdentifiersList())
    {
        m_syncActionsConfiguration->enableSyncAction(syncActionMetaData);
    }
}

void SyncActionManager::updateMasterViewerMappers()
{
    foreach (SignalToSyncActionMapper *mapper, m_registeredSignalMappers)
    {
        mapper->setViewer(m_masterViewer);
    }
}

void SyncActionManager::applySyncAction(SyncAction *syncAction)
{
    if (!syncAction)
    {
        return;
    }

    if (!m_enabled)
    {
        return;
    }
    
    foreach (QViewer *viewer, m_syncedViewersSet.values())
    {
        if (isSyncActionApplicable(syncAction, viewer))
        {
            syncAction->run(viewer);
        }
    }
}

bool SyncActionManager::isSyncActionApplicable(SyncAction *syncAction, QViewer *viewer)
{
    if (!viewer || !syncAction)
    {
        return false;
    }

    if (viewer != m_masterViewer && m_syncActionsConfiguration->isSyncActionEnabled(syncAction->getMetaData())
        && areAllCriteriaSatisfied(m_syncActionsConfiguration->getSyncCriteria(syncAction), viewer))
    {
        return true;
    }

    return false;
}

bool SyncActionManager::areAllCriteriaSatisfied(QList<SyncCriterion*> criteria, QViewer *viewer)
{
    bool criteriaAreSatisfied = true;

    int i = 0;
    while (criteriaAreSatisfied && i < criteria.count())
    {
        criteriaAreSatisfied = criteria.at(i)->isCriterionSatisfied(m_masterViewer, viewer);
        ++i;
    }

    return criteriaAreSatisfied;
}

} // End namespace udg
