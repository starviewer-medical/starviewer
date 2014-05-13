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

#include "syncactionsconfigurationhandler.h"

#include "signaltosyncactionmapperfactory.h"
#include "syncactionsconfiguration.h"
#include "syncactionmetadata.h"

#include "coresettings.h"

#include <QStringList>

namespace udg {

SyncActionsConfigurationHandler::SyncActionsConfigurationHandler()
{
}

SyncActionsConfigurationHandler::~SyncActionsConfigurationHandler()
{
}

SyncActionsConfiguration* SyncActionsConfigurationHandler::getConfiguration(const QString &context)
{
    QString key = getFormattedSettingName(context);
    
    Settings settings;
    if (!settings.contains(key))
    {
        return getDefaultSyncActionsConfiguration();
    }
    
    SyncActionsConfiguration *config = new SyncActionsConfiguration;
    QStringList enabledActions = settings.getValue(key).toString().split(";");

    foreach (const SyncActionMetaData &metaData, getRegisteredSyncActions())
    {
        if (enabledActions.contains(metaData.getSettingsName()))
        {
            config->enableSyncAction(metaData);
        }
    }
    
    return config;
}

void SyncActionsConfigurationHandler::saveConfiguration(SyncActionsConfiguration *configuration, const QString &context)
{
    if (!configuration)
    {
        return;
    }

    QStringList syncActionsSettingNames;
    QList<SyncActionMetaData> enabledSyncActions = configuration->getEnabledSyncActions();

    foreach (const SyncActionMetaData &metaData, enabledSyncActions)
    {
        syncActionsSettingNames << metaData.getSettingsName();
    }

    Settings settings;
    settings.setValue(getFormattedSettingName(context), syncActionsSettingNames.join(";"));
}

SyncActionsConfiguration* SyncActionsConfigurationHandler::getDefaultSyncActionsConfiguration()
{
    SyncActionsConfiguration *syncActionsConfiguration = new SyncActionsConfiguration();

    foreach (const SyncActionMetaData &syncActionMetaData, getRegisteredSyncActions())
    {
        syncActionsConfiguration->enableSyncAction(syncActionMetaData);
    }

    return syncActionsConfiguration;
}

QString SyncActionsConfigurationHandler::getFormattedSettingName(const QString &context)
{
    QString key;
    if (!context.isEmpty())
    {
        key = context;

        if (!key.endsWith("//"))
        {
            key += "/";
        }
    }
    key += CoreSettings::EnabledSyncActionsKey;

    return key;
}

QList<SyncActionMetaData> SyncActionsConfigurationHandler::getRegisteredSyncActions()
{
    return SignalToSyncActionMapperFactory::instance()->getFactoryIdentifiersList();
}

}
