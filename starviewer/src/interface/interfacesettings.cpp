#include "interfacesettings.h"
#include "settingsregistry.h"

#include <QDir>

namespace udg {

// Definició de les claus
const QString OpenBase("Starviewer-App-ImportFile/");
const QString InterfaceSettings::OpenFileLastPath( OpenBase + "workingDirectory" );
const QString InterfaceSettings::OpenDirectoryLastPath( OpenBase + "workingDicomDirectory" );
const QString InterfaceSettings::OpenFileLastFileExtension( OpenBase + "defaultExtension" );

const QString InterfaceSettings::ApplicationMainWindowGeometry("geometry");

const QString ExtensionsBase("Extensions/");
const QString InterfaceSettings::AllowMultipleInstancesPerExtension( ExtensionsBase + "allowMultipleExtensionInstances" );
const QString InterfaceSettings::DefaultExtension( ExtensionsBase + "defaultExtension" );

InterfaceSettings::InterfaceSettings()
{
}

InterfaceSettings::~InterfaceSettings()
{
}

void InterfaceSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    settingsRegistry->addSetting( OpenFileLastPath, QDir::homePath() );
    settingsRegistry->addSetting( OpenDirectoryLastPath, QDir::homePath()  );
    settingsRegistry->addSetting( OpenFileLastFileExtension, "MetaIO Image (*.mhd)" );
    settingsRegistry->addSetting( AllowMultipleInstancesPerExtension, false );
    settingsRegistry->addSetting( DefaultExtension, "Q2DViewerExtension" );
}

} // end namespace udg 

