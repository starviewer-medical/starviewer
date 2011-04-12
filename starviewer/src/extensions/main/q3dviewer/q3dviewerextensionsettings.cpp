#include "q3dviewerextensionsettings.h"
#include "settingsregistry.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("Starviewer-App-3DTesting/");
const QString Q3DViewerExtensionSettings::CustomClutsDirPath( KeyPrefix + "customClutsDir" );

Q3DViewerExtensionSettings::Q3DViewerExtensionSettings()
{
}

Q3DViewerExtensionSettings::~Q3DViewerExtensionSettings()
{
}

void Q3DViewerExtensionSettings::init()
{    
}

} // end namespace udg 
