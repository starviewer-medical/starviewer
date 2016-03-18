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
#include "externalapplication.h"
#include "logging.h"
#include "patient.h"
#include "volume.h"


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
    
    foreach (Settings::SettingsListItemType item, list) 
    {
        ExternalApplication::ExternalApplicationType type;
        if (item["type"] == "url")
        {
            type = ExternalApplication::ExternalApplicationType::Url;
        }
        else if (item["type"] == "cmd")
        {
            type = ExternalApplication::ExternalApplicationType::Command;
        }
        else
        {
            ERROR_LOG("Unexpected external application type");
            break;
        }
        
        ExternalApplication application(item["name"].toString(), item["url"].toString(), type);
        applications.append(application);
    }
    return applications;
}

void ExternalApplicationsManager::setApplications(const QList<ExternalApplication> &applications)
{
    Settings::SettingListType list;
    Settings settings;
    
    foreach(ExternalApplication application, applications)
    {
        Settings::SettingsListItemType item;
        item["name"] = application.getName();
        item["url"] = application.getUrl();
        item["type"] = application.getType() == ExternalApplication::ExternalApplicationType::Url ? "url" : "cmd";
        list.append(item);
    }
    
    settings.setList(CoreSettings::ExternalApplicationsConfigurationSectionName, list);
    emit onApplicationsChanged();
}

void ExternalApplicationsManager::cleanParameters()
{
    m_parameters = QHash<QString, QString>();
}

void ExternalApplicationsManager::setParameters(Volume* volume)
{
    m_parameters = QHash<QString, QString>();
    m_parameters["StudyInstanceUID"] = volume->getStudy()->getInstanceUID();
    m_parameters["SeriesInstanceUID"] = volume->getSeries()->getInstanceUID();
    m_parameters["AccessionNumber"] = volume->getStudy()->getAccessionNumber();
    m_parameters["PatientID"] = volume->getPatient()->getID();
}

const QHash<QString, QString>& ExternalApplicationsManager::getParameters() const
{
    return m_parameters;
}

void ExternalApplicationsManager::launch(const ExternalApplication &application) const
{
    application.launch(getParameters());
}

}


