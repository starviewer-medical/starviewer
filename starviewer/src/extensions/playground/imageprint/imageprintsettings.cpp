#include "imageprintsettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h" // pel UserDataRootPath

namespace udg {

// Definició de les claus
const QString DicomPrint("DicomPrint/");
const QString ImagePrintSettings::SpoolDirectory( DicomPrint + "SpoolDirectory");

ImagePrintSettings::ImagePrintSettings()
{
}

ImagePrintSettings::~ImagePrintSettings()
{
}

void ImagePrintSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    
    //especifiquem el directori spool per defecte
    settingsRegistry->addSetting(SpoolDirectory, UserDataRootPath + "spool", Settings::Parseable);
}

} // end namespace udg 

