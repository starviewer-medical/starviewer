#include "coresettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h"
#include <QDir>
#include <QLocale> // pel languageLocaleKey

namespace udg {

// Definició de les claus
const QString screenShotToolBaseKey("ScreenshotTool/");
const QString CoreSettings::screenShotToolFolderKey(screenShotToolBaseKey + "defaultSaveFolder");
const QString CoreSettings::screenShotToolFileExtensionKey(screenShotToolBaseKey + "defaultSaveExtension");
const QString CoreSettings::screenShotToolFilenameKey(screenShotToolBaseKey + "defaultSaveName");

const QString CoreSettings::logViewerGeometryKey("logViewerDialogGeometry");
const QString CoreSettings::userHangingProtocolsPathKey("Hanging-Protocols/path");
const QString CoreSettings::registerStatLogsKey("LoggingPolicy/registerStatistics");

// Paràmetres d'obscurances
const QString obscurancesBaseKey("3DViewer/obscurances/");
const QString lowQualityObscurancesBaseKey( obscurancesBaseKey + "low/" );
const QString mediumQualityObscurancesBaseKey( obscurancesBaseKey + "medium/" );
const QString highQualityObscurancesBaseKey( obscurancesBaseKey + "high/" );

const QString CoreSettings::numberOfDirectionsForLowQualityObscurancesKey( lowQualityObscurancesBaseKey + "numberOfDirections" );
const QString CoreSettings::functionForLowQualityObscurancesKey( lowQualityObscurancesBaseKey + "function" );
const QString CoreSettings::variantForLowQualityObscurancesKey( lowQualityObscurancesBaseKey + "variant" );
const QString CoreSettings::gradientRadiusForLowQualityObscurancesKey( lowQualityObscurancesBaseKey + "gradientRadius" );

const QString CoreSettings::numberOfDirectionsForMediumQualityObscurancesKey( mediumQualityObscurancesBaseKey + "numberOfDirections" );
const QString CoreSettings::functionForMediumQualityObscurancesKey( mediumQualityObscurancesBaseKey + "function" );
const QString CoreSettings::variantForMediumQualityObscurancesKey( mediumQualityObscurancesBaseKey + "variant" );
const QString CoreSettings::gradientRadiusForMediumQualityObscurancesKey( mediumQualityObscurancesBaseKey + "gradientRadius" );

const QString CoreSettings::numberOfDirectionsForHighQualityObscurancesKey( highQualityObscurancesBaseKey + "numberOfDirections" );
const QString CoreSettings::functionForHighQualityObscurancesKey( highQualityObscurancesBaseKey + "function" );
const QString CoreSettings::variantForHighQualityObscurancesKey( highQualityObscurancesBaseKey + "variant" );
const QString CoreSettings::gradientRadiusForHighQualityObscurancesKey( highQualityObscurancesBaseKey + "gradientRadius" );

const QString CoreSettings::languageLocaleKey("Starviewer-Language/languageLocale");

CoreSettings::CoreSettings()
{
}

CoreSettings::~CoreSettings()
{
}

void CoreSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    settingsRegistry->addSetting( screenShotToolFolderKey, QDir::homePath() );
    settingsRegistry->addSetting( screenShotToolFileExtensionKey, QObject::tr("PNG (*.png)") );
    settingsRegistry->addSetting( userHangingProtocolsPathKey, UserHangingProtocolsPath );
    settingsRegistry->addSetting( registerStatLogsKey, false );
    settingsRegistry->addSetting( languageLocaleKey, QLocale::system().name() );
    
}

} // end namespace udg 

