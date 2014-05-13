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

#ifndef UDGSYNCACTIONSCONFIGURATIONMENU_H
#define UDGSYNCACTIONSCONFIGURATIONMENU_H

#include <QSignalMapper>
#include <QMenu>
#include <QAction>

namespace udg {

class SyncActionsConfiguration;
class SyncActionMetaData;

/**
    QMenu to enable or disable SyncActions according to the given SyncActionsConfiguration
 */
class SyncActionsConfigurationMenu : public QMenu {
Q_OBJECT
public:
    SyncActionsConfigurationMenu(SyncActionsConfiguration *config, QWidget *parent = 0);
    ~SyncActionsConfigurationMenu();

private:
    /// Creates the QAction corresponding to the given SyncActionMetaData according to its configuration
    QAction* getQAction(const SyncActionMetaData &syncActionMetaData);

private slots:
    /// Used by the signal mapper to identify which sync action QAction in the menu has been toggled
    void toggled(const QString &name);

private:
    /// The SyncActionsConfiguration to handle in the menu
    SyncActionsConfiguration *m_syncActionsConfig;
    
    /// Signal mapper to map the toggle signal to the corresponding sync action string identifier
    QSignalMapper *m_signalMapper;

    /// Map to bind sync actions to their corresponding QAction in the menu
    QMap<SyncActionMetaData, QAction*> m_syncActionsMap;
};

} // End namespace udg

#endif
