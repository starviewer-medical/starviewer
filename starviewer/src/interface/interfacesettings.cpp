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

#include "interfacesettings.h"
#include "settingsregistry.h"

#include <QDir>

namespace udg {

// Definició de les claus
const QString OpenBase("Starviewer-App-ImportFile/");
const QString InterfaceSettings::OpenFileLastPath(OpenBase + "workingDirectory");
const QString InterfaceSettings::OpenDirectoryLastPath(OpenBase + "workingDicomDirectory");
const QString InterfaceSettings::OpenFileLastFileExtension(OpenBase + "defaultExtension");

const QString InterfaceSettings::ApplicationMainWindowGeometry("geometry");

const QString ExtensionsBase("Extensions/");
const QString InterfaceSettings::AllowMultipleInstancesPerExtension(ExtensionsBase + "allowMultipleExtensionInstances");
const QString InterfaceSettings::DefaultExtension(ExtensionsBase + "defaultExtension");

InterfaceSettings::InterfaceSettings()
{
}

InterfaceSettings::~InterfaceSettings()
{
}

void InterfaceSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    settingsRegistry->addSetting(OpenFileLastPath, QDir::homePath());
    settingsRegistry->addSetting(OpenDirectoryLastPath, QDir::homePath());
    settingsRegistry->addSetting(OpenFileLastFileExtension, "MetaIO Image (*.mhd)");
    settingsRegistry->addSetting(AllowMultipleInstancesPerExtension, false);
    settingsRegistry->addSetting(DefaultExtension, "Q2DViewerExtension");
}

} // end namespace udg
