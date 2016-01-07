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

#include "syncactionmanager.h"

#include "signaltosyncactionmapperfactory.h"
#include "syncactionsconfigurationhandler.h"
#include "syncaction.h"
#include "synccriterion.h"
#include "syncactionsconfiguration.h"

#include "q2dviewer.h"

namespace udg {

SyncActionManager::SyncActionManager(SyncActionsConfiguration *configuration, QObject *parent)
 : QObject(parent)
{
    m_masterViewer = 0;
    m_enabled = false;
    m_synchronizingAll = false;
    
    setupSignalMappers();
    setupSyncActionsConfiguration(configuration);
}

SyncActionManager::~SyncActionManager()
{
    delete m_syncActionsConfiguration;

    foreach (SignalToSyncActionMapper *mapper, m_registeredSignalMappers)
    {
        delete mapper;
    }
}

void SyncActionManager::addSyncedViewer(QViewer *viewer)
{
    if (!m_syncedViewersList.contains(viewer))
    {
        m_syncedViewersList << viewer;

        Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
        if (viewer2D)
        {
            connect(viewer2D, SIGNAL(restored()), this, SLOT(synchronize()));
            connect(viewer2D, SIGNAL(anatomicalViewChanged(AnatomicalPlane)), this, SLOT(synchronizeAllViewersButSender()));
            connect(viewer2D, SIGNAL(newVolumesRendered()), this, SLOT(synchronizeAllViewersButSender()));
        }
    }
}

void SyncActionManager::removeSyncedViewer(QViewer *viewer)
{
    if (viewer == m_masterViewer)
    {
        m_masterViewer = 0;
        updateMasterViewerMappers();
    }

    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
    if (viewer2D)
    {
        disconnect(viewer2D, 0, this, 0);
    }

    m_syncedViewersList.removeOne(viewer);
}

void SyncActionManager::setMasterViewer(QViewer *viewer)
{
    if (m_syncedViewersList.contains(viewer))
    {
        m_masterViewer = viewer;
        updateMasterViewerMappers();
    }
}

void SyncActionManager::clearSyncedViewersSet()
{
    m_syncedViewersList.clear();
}

void SyncActionManager::setSyncActionsConfiguration(SyncActionsConfiguration *configuration)
{
    if (!configuration)
    {
        return;
    }

    delete m_syncActionsConfiguration;
    
    m_syncActionsConfiguration = configuration;
}

SyncActionsConfiguration* SyncActionManager::getSyncActionsConfiguration()
{
    return m_syncActionsConfiguration;
}

bool SyncActionManager::isEnabled() const
{
    return m_enabled;
}

void SyncActionManager::enable(bool enable)
{
    m_enabled = enable;

    if (m_enabled && m_masterViewer)
    {
        synchronizeAll();
    }
}

void SyncActionManager::synchronize()
{
    if (m_masterViewer->getViewerStatus() != QViewer::VisualizingVolume)
    {
        return;
    }

    foreach (SignalToSyncActionMapper *mapper, m_registeredSignalMappers)
    {
        mapper->mapProperty();
    }
}

void SyncActionManager::synchronizeAll()
{
    synchronizeAllWithExceptions(QSet<QViewer*>());
}

void SyncActionManager::synchronizeAllViewersButSender()
{
    QViewer *senderViewer = qobject_cast<QViewer*>(sender());

    if (!senderViewer)
    {
        return;
    }

    synchronizeAllWithExceptions(QSet<QViewer*>() << senderViewer);
}

void SyncActionManager::synchronizeAllWithExceptions(QSet<QViewer*> excludedViewers)
{
    QViewer *selectedViewer = m_masterViewer;
    m_syncActionsAppliedPerViewer.clear();
    m_synchronizingAll = true;

    // Selected viewer is syncronized first
    if (selectedViewer && selectedViewer->getMainInput() && !excludedViewers.contains(selectedViewer))
    {
        this->synchronize();
    }

    foreach (QViewer *currentViewer, m_syncedViewersList)
    {
        if (currentViewer != selectedViewer && currentViewer->getMainInput() && !excludedViewers.contains(currentViewer))
        {
            this->setMasterViewer(currentViewer);
            this->synchronize();
        }
    }

    m_synchronizingAll = false;

    this->setMasterViewer(selectedViewer);
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

void SyncActionManager::setupSyncActionsConfiguration(SyncActionsConfiguration *configuration)
{
    if (!configuration)
    {
        m_syncActionsConfiguration = SyncActionsConfigurationHandler::getDefaultSyncActionsConfiguration();
    }
    else
    {
        m_syncActionsConfiguration = configuration;
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

    QString syncActionName = syncAction->getMetaData().getSettingsName();

    if (!m_synchronizingAll || !m_syncActionsAppliedPerViewer.contains(syncActionName, m_masterViewer))
    {
        foreach (QViewer *viewer, m_syncedViewersList)
        {
            if (isSyncActionApplicable(syncAction, viewer))
            {
                syncAction->run(viewer);

                if (m_synchronizingAll)
                {
                    m_syncActionsAppliedPerViewer.insert(syncActionName, viewer);
                }
            }
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
