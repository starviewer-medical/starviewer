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

#ifndef UDGSYNCACTIONSCONFIGURATION_H
#define UDGSYNCACTIONSCONFIGURATION_H

#include <QSet>
#include <QMultiMap>

namespace udg {

class SyncActionMetaData;
class SyncAction;
class SyncCriterion;

/**
    This class enables defining contextualized configurations of the available SyncActions
 */
class SyncActionsConfiguration {
public:
    SyncActionsConfiguration();
    ~SyncActionsConfiguration();

    /// Registers the given type of sync action as enabled or disabled whether enable parameter is true or false, respectively
    void enableSyncAction(const SyncActionMetaData &syncActionMetaData, bool enable = true);
    
    /// Returns true if the given SyncAction is enabled
    bool isSyncActionEnabled(const SyncActionMetaData &syncActionMetaData);

    /// Returns the list of SyncActions that are enabled
    QList<SyncActionMetaData> getEnabledSyncActions() const;

    /// Returns the corresponding list of SyncCriterion defined for a SyncAction.
    /// If current configuration has no SyncCriterion defined for the given SyncAction, default from own SyncAction will be returned.
    QList<SyncCriterion*> getSyncCriteria(SyncAction *syncAction);

    /// Adds a SyncCriterion for a determined SyncAction
    void addSyncCriterion(const SyncActionMetaData &syncActionMetaData, SyncCriterion *criterion);
    
    /// Deletes all the SyncCriterion for a determined SyncAction
    void removeSyncCriteria(const SyncActionMetaData &syncActionMetaData);

private:
    /// Deletes all the pointers in the configured sync criteria map. To be called on destructor only to avoid memory leaks.
    void cleanupConfiguredSyncCriteria();

private:
    /// Map containing the enabled sync actions indexed by the identifier name of the SyncAction
    QMap<QString, SyncActionMetaData> m_enabledSyncActions;

    /// This map contains which SyncCriterion corresponds to an specific SyncAction through its metadata
    /// When SyncCriterion are defined for a SyncAction in this map, they'll have priority over own SyncAction default defined criteria
    QMultiMap<SyncActionMetaData, SyncCriterion*> m_configuredSyncCriteria;
};

} // End namespace udg

#endif
