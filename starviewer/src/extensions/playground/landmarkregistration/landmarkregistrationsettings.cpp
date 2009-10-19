#include "landmarkregistrationsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("StarViewer-App-LandmarkRegistration/");
const QString LandmarkRegistrationSettings::VerticalSplitterGeometry( KeyPrefix + "verticalSplitter" );
const QString LandmarkRegistrationSettings::VerticalSplitter2Geometry( KeyPrefix + "verticalSplitter2" );
const QString LandmarkRegistrationSettings::SavedFilesPath( KeyPrefix + "savingDirectory" );

LandmarkRegistrationSettings::LandmarkRegistrationSettings()
{
}

LandmarkRegistrationSettings::~LandmarkRegistrationSettings()
{
}

void LandmarkRegistrationSettings::init()
{    
}

} // end namespace udg 