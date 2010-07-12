#include "coresettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h"
#include <QDir>
#include <QLocale> // pel LanguageLocale

namespace udg {

// Definició de les claus
const QString ScreenShotToolBase("ScreenshotTool/");
const QString CoreSettings::ScreenShotToolFolder(ScreenShotToolBase + "defaultSaveFolder");
const QString CoreSettings::ScreenShotToolFileExtension(ScreenShotToolBase + "defaultSaveExtension");
const QString CoreSettings::ScreenShotToolFilename(ScreenShotToolBase + "defaultSaveName");

const QString CoreSettings::LogViewerGeometry("logViewerDialogGeometry");
const QString CoreSettings::UserHangingProtocolsPath("Hanging-Protocols/path");
const QString CoreSettings::RegisterStatLogs("LoggingPolicy/registerStatistics");

// Mida per defecte de la font de les annotacions
const QString CoreSettings::DefaultToolTextSize("Tools/DefaultTextSize");

// Paràmetres d'obscurances
const QString ObscurancesBase("3DViewer/obscurances/");
const QString LowQualityObscurancesBase( ObscurancesBase + "low/" );
const QString MediumQualityObscurancesBase( ObscurancesBase + "medium/" );
const QString HighQualityObscurancesBase( ObscurancesBase + "high/" );

const QString CoreSettings::NumberOfDirectionsForLowQualityObscurances( LowQualityObscurancesBase + "numberOfDirections" );
const QString CoreSettings::FunctionForLowQualityObscurances( LowQualityObscurancesBase + "function" );
const QString CoreSettings::VariantForLowQualityObscurances( LowQualityObscurancesBase + "variant" );
const QString CoreSettings::GradientRadiusForLowQualityObscurances( LowQualityObscurancesBase + "gradientRadius" );

const QString CoreSettings::NumberOfDirectionsForMediumQualityObscurances( MediumQualityObscurancesBase + "numberOfDirections" );
const QString CoreSettings::FunctionForMediumQualityObscurances( MediumQualityObscurancesBase + "function" );
const QString CoreSettings::VariantForMediumQualityObscurances( MediumQualityObscurancesBase + "variant" );
const QString CoreSettings::GradientRadiusForMediumQualityObscurances( MediumQualityObscurancesBase + "gradientRadius" );

const QString CoreSettings::NumberOfDirectionsForHighQualityObscurances( HighQualityObscurancesBase + "numberOfDirections" );
const QString CoreSettings::FunctionForHighQualityObscurances( HighQualityObscurancesBase + "function" );
const QString CoreSettings::VariantForHighQualityObscurances( HighQualityObscurancesBase + "variant" );
const QString CoreSettings::GradientRadiusForHighQualityObscurances( HighQualityObscurancesBase + "gradientRadius" );

const QString CoreSettings::LanguageLocale("Starviewer-Language/languageLocale");

const QString CoreSettings::DICOMImageReaderLibrary("Input/DICOMImageReaderLibrary");

//Release Notes
const QString CoreSettings::ShowReleaseNotesFirstTime("ShowReleaseNotesFirstTime");
const QString CoreSettings::NeverShowReleaseNotes("NeverShowReleaseNotes");

CoreSettings::CoreSettings()
{
}

CoreSettings::~CoreSettings()
{
}

void CoreSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    settingsRegistry->addSetting( ScreenShotToolFolder, QDir::homePath() );
    settingsRegistry->addSetting( ScreenShotToolFileExtension, QObject::tr("PNG (*.png)") );
    settingsRegistry->addSetting( UserHangingProtocolsPath, UserHangingProtocolsPath );
    settingsRegistry->addSetting( RegisterStatLogs, false );
    settingsRegistry->addSetting( LanguageLocale, QLocale::system().name() );
    settingsRegistry->addSetting( DefaultToolTextSize, 14 );
    settingsRegistry->addSetting(DICOMImageReaderLibrary, "itkGDCM");
    settingsRegistry->addSetting(ShowReleaseNotesFirstTime, true);
    settingsRegistry->addSetting(NeverShowReleaseNotes, false);
}

} // end namespace udg 

