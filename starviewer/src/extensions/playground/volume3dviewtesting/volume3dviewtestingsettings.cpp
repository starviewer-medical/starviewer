#include "volume3dviewtestingsettings.h"
#include "settingsregistry.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("Starviewer-App-3DTesting/");
const QString Volume3DViewTestingSettings::CustomClutsDirPath( KeyPrefix + "customClutsDir" );

Volume3DViewTestingSettings::Volume3DViewTestingSettings()
{
}

Volume3DViewTestingSettings::~Volume3DViewTestingSettings()
{
}

void Volume3DViewTestingSettings::init()
{    
}

} // end namespace udg 
