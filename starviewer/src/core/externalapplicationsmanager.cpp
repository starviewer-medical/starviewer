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

#include "externalapplicationsmanager.h"
#include "coresettings.h"

namespace udg {

ExternalApplicationsManager::ExternalApplicationsManager(QObject *parent) : QObject(parent)
{

}

ExternalApplicationsManager::~ExternalApplicationsManager()
{
    Settings settings;
    Settings::SettingListType list = settings.getList(CoreSettings::ExternalApplicationsConfigurationSectionName);
}

QList<ExternalApplication> ExternalApplicationsManager::getApplications() const
{
    QList<ExternalApplication> applications;
    Settings settings;
    Settings::SettingListType list = settings.getList(CoreSettings::ExternalApplicationsConfigurationSectionName);
    foreach (Settings::SettingsListItemType item, list) {
        ExternalApplication application(item["name"].toString(),item["url"].toString());
        applications.append(application);
    }
    return applications;
}

void ExternalApplicationsManager::setApplications(const QList<ExternalApplication>& applications)
{
    Settings::SettingListType list;
    Settings settings;
    foreach(ExternalApplication application, applications) {
        Settings::SettingsListItemType item;
        item["name"] = application.getName();
        item["url"] = application.getUrl();
        list.append(item);
    }
    settings.setList(CoreSettings::ExternalApplicationsConfigurationSectionName, list);
    emit onApplicationsChanged();
}


}


