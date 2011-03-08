#include "coresettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h"
#include <QDir>
#include <QLocale> // Pel LanguageLocale
#include <QStringList>

namespace udg {

// Definició de les claus
const QString ScreenShotToolBase("ScreenshotTool/");
const QString CoreSettings::ScreenShotToolFolder(ScreenShotToolBase + "defaultSaveFolder");
const QString CoreSettings::ScreenShotToolFileExtension(ScreenShotToolBase + "defaultSaveExtension");
const QString CoreSettings::ScreenShotToolFilename(ScreenShotToolBase + "defaultSaveName");

const QString CoreSettings::LogViewerGeometry("logViewerDialogGeometry");
const QString CoreSettings::UserHangingProtocolsPath("Hanging-Protocols/path");
const QString CoreSettings::UserDICOMDumpDefaultTagsPath("DefaultTags/path");
const QString CoreSettings::UserCustomWindowLevelsPath("CustomWindowLevels/path");
const QString CoreSettings::RegisterStatLogs("LoggingPolicy/registerStatistics");

const QString CoreSettings::ToolsFontSize("Tools/ToolsFontSize");
const QString CoreSettings::ScaledUserInterfaceFontSize("ScaledUserInterfaceFontSize");

// Paràmetres d'obscurances
const QString ObscurancesBase("3DViewer/obscurances/");
const QString LowQualityObscurancesBase(ObscurancesBase + "low/");
const QString MediumQualityObscurancesBase(ObscurancesBase + "medium/");
const QString HighQualityObscurancesBase(ObscurancesBase + "high/");

const QString CoreSettings::NumberOfDirectionsForLowQualityObscurances(LowQualityObscurancesBase + "numberOfDirections");
const QString CoreSettings::FunctionForLowQualityObscurances(LowQualityObscurancesBase + "function");
const QString CoreSettings::VariantForLowQualityObscurances(LowQualityObscurancesBase + "variant");
const QString CoreSettings::GradientRadiusForLowQualityObscurances(LowQualityObscurancesBase + "gradientRadius");

const QString CoreSettings::NumberOfDirectionsForMediumQualityObscurances(MediumQualityObscurancesBase + "numberOfDirections");
const QString CoreSettings::FunctionForMediumQualityObscurances(MediumQualityObscurancesBase + "function");
const QString CoreSettings::VariantForMediumQualityObscurances(MediumQualityObscurancesBase + "variant");
const QString CoreSettings::GradientRadiusForMediumQualityObscurances(MediumQualityObscurancesBase + "gradientRadius");

const QString CoreSettings::NumberOfDirectionsForHighQualityObscurances(HighQualityObscurancesBase + "numberOfDirections");
const QString CoreSettings::FunctionForHighQualityObscurances(HighQualityObscurancesBase + "function");
const QString CoreSettings::VariantForHighQualityObscurances(HighQualityObscurancesBase + "variant");
const QString CoreSettings::GradientRadiusForHighQualityObscurances(HighQualityObscurancesBase + "gradientRadius");

const QString CoreSettings::LanguageLocale("Starviewer-Language/languageLocale");

const QString CoreSettings::ForcedImageReaderLibrary("Input/ForcedImageReaderLibrary");
const QString CoreSettings::ForceITKImageReaderForSpecifiedModalities("Input/ForceITKImageReaderForSpecifiedModalities");
const QString CoreSettings::ForceVTKImageReaderForSpecifiedModalities("Input/ForceVTKImageReaderForSpecifiedModalities");

// Release Notes
const QString CoreSettings::LastReleaseNotesVersionShown("LastReleaseNotesVersionShown");

// Comprobar noves versions
const QString CoreSettings::LastVersionChecked("LastVersionChecked");
const QString CoreSettings::LastVersionCheckedDate("LastVersionCheckedDate");
const QString CoreSettings::CheckVersionInterval("CheckVersionInterval");
const QString CoreSettings::NeverShowNewVersionReleaseNotes("NeverShowNewVersionReleaseNotes");
const QString CoreSettings::DontCheckNewVersionsOnline("DontCheckNewVersionsOnline");

const QString CoreSettings::MammographyAutoOrientationExceptions("MammographyAutoOrientationExceptions");

const QString CoreSettings::AllowAsynchronousVolumeLoading("AllowAsynchronousVolumeLoading");

const QString CoreSettings::MaximumNumberOfVisibleWindowLevelComboItems("MaximumNumberOfVisibleWindowLevelComboItems");

// Geometria de la finestra del QDICOMDumpBrowser
const QString CoreSettings::QDICOMDumpBrowserGeometry("QDICOMDumpBrowserGeometry");

CoreSettings::CoreSettings()
{
}

CoreSettings::~CoreSettings()
{
}

void CoreSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    settingsRegistry->addSetting(ScreenShotToolFolder, QDir::homePath());
    settingsRegistry->addSetting(ScreenShotToolFileExtension, QObject::tr("PNG (*.png)"));
    settingsRegistry->addSetting(UserHangingProtocolsPath, UserDataRootPath + "hangingprotocols/");
    settingsRegistry->addSetting(UserDICOMDumpDefaultTagsPath, UserDataRootPath + "dicomdumpdefaulttags/");
    settingsRegistry->addSetting(UserCustomWindowLevelsPath, UserDataRootPath + "customwindowlevels/customwindowlevels.xml");
    settingsRegistry->addSetting(RegisterStatLogs, false);
    settingsRegistry->addSetting(LanguageLocale, QLocale::system().name());
    settingsRegistry->addSetting(LastReleaseNotesVersionShown, "");
    settingsRegistry->addSetting(NeverShowNewVersionReleaseNotes, false);
    settingsRegistry->addSetting(LastVersionChecked, "");
    settingsRegistry->addSetting(LastVersionCheckedDate, "");
    settingsRegistry->addSetting(CheckVersionInterval, "1");
    settingsRegistry->addSetting(DontCheckNewVersionsOnline, false);
    settingsRegistry->addSetting(MammographyAutoOrientationExceptions, (QStringList() << "BAV" << "BAG" << "estereot"));
    settingsRegistry->addSetting(AllowAsynchronousVolumeLoading, true);
    settingsRegistry->addSetting(MaximumNumberOfVisibleWindowLevelComboItems, 50);
}

} // End namespace udg 

