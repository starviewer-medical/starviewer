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

#ifndef UDGSYNCACTIONSCONFIGURATIONHANDLER_H
#define UDGSYNCACTIONSCONFIGURATIONHANDLER_H

#include <QList>

class QString;

namespace udg {

class SyncActionsConfiguration;
class SyncActionMetaData;

/**
    Class to handle sync actions configurations. It lets save to and retrieve from settings, giving a specific context,
    then several configurations can be handled for diferent contexts if needed.
 */
class SyncActionsConfigurationHandler {
public:
    SyncActionsConfigurationHandler();
    ~SyncActionsConfigurationHandler();

    /// Gets the configuration corresponding to the given settings context. If context does not exists,
    /// the resulting configuration will be the same as getDefaultSyncActionsConfiguration()
    SyncActionsConfiguration* getConfiguration(const QString &context);

    /// Saves the given configuration in settings under the given named context
    void saveConfiguration(SyncActionsConfiguration *configuration, const QString &context);
    
    /// Gets the default sync actions configuration, which corresponds to all the registered sync actions
    static SyncActionsConfiguration* getDefaultSyncActionsConfiguration();

private:
    /// Gets the right formatted setting name
    QString getFormattedSettingName(const QString &context);

    /// Gets the list of registered sync actions
    static QList<SyncActionMetaData> getRegisteredSyncActions();
};

} // End namespace udg

#endif
