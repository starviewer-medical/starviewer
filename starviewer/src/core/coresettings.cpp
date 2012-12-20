#include "coresettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h"
#include <QDir>
// Pel LanguageLocale
#include <QLocale>
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

const QString ToolsBase("Tools/");
const QString CoreSettings::ToolsFontSize(ToolsBase + "ToolsFontSize");
const QString CoreSettings::ScaledUserInterfaceFontSize("ScaledUserInterfaceFontSize");

const QString CoreSettings::MagnifyingGlassZoomFactor(ToolsBase + "MagnifyingGlassZoomFactor");

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
const QString CoreSettings::MaximumNumberOfVolumesLoadingConcurrently("MaximumNumberOfVolumesLoadingConcurrently");

const QString CoreSettings::MaximumNumberOfVisibleWindowLevelComboItems("MaximumNumberOfVisibleWindowLevelComboItems");

// Geometria de la finestra del QDICOMDumpBrowser
const QString CoreSettings::QDICOMDumpBrowserGeometry("QDICOMDumpBrowserGeometry");

//Llista de PACS per defecte
const QString CoreSettings::DefaultPACSListToQuery("PACS/defaultPACSListToQuery");
//TODO:Aquesta clau està duplicada a InputOutputSettings
const QString CoreSettings::PacsListConfigurationSectionName = "PacsList";

const QString Q2DViewerBase("2DViewer/");
const QString CoreSettings::EnableQ2DViewerSliceScrollLoop(Q2DViewerBase + "enable2DViewerSliceScrollLoop");
const QString CoreSettings::EnableQ2DViewerPhaseScrollLoop(Q2DViewerBase + "enable2DViewerPhaseScrollLoop");
const QString CoreSettings::EnableQ2DViewerReferenceLinesForMR(Q2DViewerBase + "enable2DViewerReferenceLinesForMR");
const QString CoreSettings::EnableQ2DViewerReferenceLinesForCT(Q2DViewerBase + "enable2DViewerReferenceLinesForCT");
const QString CoreSettings::ModalitiesWithZoomToolByDefault(Q2DViewerBase + "ModalitiesWithZoomToolByDefault");
const QString CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR(Q2DViewerBase + "enable2DViewerAutomaticSynchronizationForMR");
const QString CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT(Q2DViewerBase + "enable2DViewerAutomaticSynchronizationForCT");
const QString CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption(Q2DViewerBase + "modalitiesToApplyHangingProtocolsAsFirstOption");
const QString CoreSettings::StudyLayoutConfigList(Q2DViewerBase + "StudyLayoutConfigList");

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
    settingsRegistry->addSetting(MagnifyingGlassZoomFactor, "4");
    settingsRegistry->addSetting(LanguageLocale, QLocale::system().name());
    settingsRegistry->addSetting(LastReleaseNotesVersionShown, "");
    settingsRegistry->addSetting(NeverShowNewVersionReleaseNotes, false);
    settingsRegistry->addSetting(LastVersionChecked, "");
    settingsRegistry->addSetting(LastVersionCheckedDate, "");
    settingsRegistry->addSetting(CheckVersionInterval, "1");
    settingsRegistry->addSetting(DontCheckNewVersionsOnline, false);
    settingsRegistry->addSetting(MammographyAutoOrientationExceptions, (QStringList() << "BAV" << "BAG" << "estereot"));
    settingsRegistry->addSetting(AllowAsynchronousVolumeLoading, true);
    settingsRegistry->addSetting(MaximumNumberOfVolumesLoadingConcurrently, 1);
    settingsRegistry->addSetting(MaximumNumberOfVisibleWindowLevelComboItems, 50);
    settingsRegistry->addSetting(EnableQ2DViewerSliceScrollLoop, false);
    settingsRegistry->addSetting(EnableQ2DViewerPhaseScrollLoop, false);
    settingsRegistry->addSetting(EnableQ2DViewerReferenceLinesForMR, true);
    settingsRegistry->addSetting(EnableQ2DViewerReferenceLinesForCT, false);
    settingsRegistry->addSetting(ModalitiesWithZoomToolByDefault, "MG;CR;RF;OP;DX;MR");
    settingsRegistry->addSetting(EnableQ2DViewerAutomaticSynchronizationForMR, true);
    settingsRegistry->addSetting(EnableQ2DViewerAutomaticSynchronizationForCT, true);
    // TODO Encara falta determinar quines seran les modalitats en les que posarem per defecte aplicar sempre hanging protocols
    settingsRegistry->addSetting(ModalitiesToApplyHangingProtocolsAsFirstOption, "CR;CT;MG;MR;US");
}

} // End namespace udg
