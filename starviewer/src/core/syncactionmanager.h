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

#ifndef UDGSYNCACTIONMANAGER_H
#define UDGSYNCACTIONMANAGER_H

#include <QObject>
#include <QList>
#include <QMultiHash>

namespace udg {

class QViewer;
class SignalToSyncActionMapper;
class SyncAction;
class SyncCriterion;
class SyncActionsConfiguration;

/**
    Class to manage SyncActions over a set of viewers.
    There will be a viewer that will act as a master viewer, set through setMasterViewer(),
    whose actions will be propagated to the rest of the registered viewers
    We can also configure which SyncActions will be propagated via setSyncActionsConfiguration()
    By default, if no configiration is provided, all registered SyncActions will be enabled
 */
class SyncActionManager : public QObject {
Q_OBJECT
public:
    SyncActionManager(SyncActionsConfiguration *configuration, QObject *parent = 0);
    ~SyncActionManager();

    /// Adds the given viewer to the set of synced viewers
    void addSyncedViewer(QViewer *viewer);

    /// Removes the given viewer from the set of synced viewers
    void removeSyncedViewer(QViewer *viewer);

    /// Sets the given viewer as the master viewer. The viewer must be already added in the set.
    void setMasterViewer(QViewer *viewer);
    
    /// Clears the synced viewers set
    void clearSyncedViewersSet();

    /// Sets the configuration for the registered sync actions
    void setSyncActionsConfiguration(SyncActionsConfiguration *configuration);

    /// Gets the current sync actions configuration
    SyncActionsConfiguration* getSyncActionsConfiguration();

    /// Returns true if the manager is enabled and false otherwise.
    bool isEnabled() const;

public slots:
    /// Enables or disables the manager. When disabled, no actions will be applied on the viewers set.
    /// The viewers set will always remain unaltered, wheter the manager is disabled or enabled.
    /// By default the manager will be disabled
    void enable(bool enable);

    /// Force all mappers to map the property even no signal was received
    void synchronize();

private:
    /// Sets up the signal mappers that will generate the SyncActions
    void setupSignalMappers();
    
    /// Sets up the configuration for sync actions upon the given one. It enables all the registered sync actions if null
    void setupSyncActionsConfiguration(SyncActionsConfiguration *configuration);
    
    /// Sets the current master viewer on the registered signal mappers
    void updateMasterViewerMappers();

    /// Checks if the given SyncAction is applicable on the given QViewer
    bool isSyncActionApplicable(SyncAction *syncAction, QViewer *viewer);

    /// Returns true if all the criteria from the list are met between the given viewer and the master one
    bool areAllCriteriaSatisfied(QList<SyncCriterion*> criteria, QViewer *viewer);

    /// Synchronize all viewers. The master viewer is synchronized first.
    void synchronizeAll();

    /// Synchronize all viewers except the set of viewers given as parameter. The master viewer is synchronized first if it is not in the list.
    void synchronizeAllWithExceptions(QSet<QViewer*> excludedViewers);

private slots:
    /// Applies the given SyncAction on the registered viewers, but the master viewer
    void applySyncAction(SyncAction *syncAction);

    /// Synchronize all viewers except the sender. The master viewer is synchronized first.
    void synchronizeAllViewersButSender();

private:
    /// The list of viewers to be synced
    QList<QViewer*> m_syncedViewersList;
    
    /// The list of registered signal mappers that could be used to propagate the SyncActions
    QList<SignalToSyncActionMapper*> m_registeredSignalMappers;
    
    /// The master viewer that will set which actions should be propagated among the rest
    QViewer *m_masterViewer;

    /// Attribute that holds which sync actions are enabled or not.
    /// By default, if no SyncActionsConfiguration is provided, all registered SyncActions will be enabled
    SyncActionsConfiguration *m_syncActionsConfiguration;

    /// Attribute to hold if appliance of sync actions is enabled or not
    bool m_enabled;

    /// Helper attributes to avoid unnecessary syncronizations when syncronizing all viewers
    QMultiHash<QString, QViewer*> m_syncActionsAppliedPerViewer;
    bool m_synchronizingAll;
};

} // End namespace udg

#endif
