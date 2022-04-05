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

#include "coresettings.h"

#include "q2dviewerannotationssettingshelper.h"
#include "settingsregistry.h"
#include "starviewerapplication.h"

#include <QDir>
// Pel LanguageLocale
#include <QLocale>
#include <QStringList>
#include <QVector>

namespace udg {

namespace {

const QVector<QString> ComparisonModeDivisionStrings{"CurrentLeftPriorRight", "CurrentRightPriorLeft", "CurrentTopPriorBottom", "CurrentBottomPriorTop"};

}

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
const QString CoreSettings::MagnifyingGlassZoomFactor(ToolsBase + "MagnifyingGlassZoomFactor");

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
const QString CoreSettings::UpdateCheckUrlAdditionalParameters("UpdateCheckUrlAdditionalParameters");

const QString CoreSettings::MammographyAutoOrientationExceptions("MammographyAutoOrientationExceptions");

const QString CoreSettings::AllowAsynchronousVolumeLoading("AllowAsynchronousVolumeLoading");
const QString CoreSettings::MaximumNumberOfVolumesLoadingConcurrently("MaximumNumberOfVolumesLoadingConcurrently");

const QString CoreSettings::MaximumNumberOfVisibleVoiLutComboItems("MaximumNumberOfVisibleVoiLutComboItems");

// Geometria de la finestra del QDICOMDumpBrowser
const QString CoreSettings::QDICOMDumpBrowserGeometry("QDICOMDumpBrowserGeometry");

//Llista de PACS per defecte
const QString CoreSettings::DefaultPACSListToQuery("PACS/defaultPACSListToQuery");

const QString CoreSettings::ExternalApplicationsConfigurationSectionName = "ExternalApplications";

const QString Q2DViewerBase("2DViewer/");
const QString CoreSettings::EnableQ2DViewerSliceScrollLoop(Q2DViewerBase + "enable2DViewerSliceScrollLoop");
const QString CoreSettings::EnableQ2DViewerPhaseScrollLoop(Q2DViewerBase + "enable2DViewerPhaseScrollLoop");
const QString CoreSettings::EnableQ2DViewerWheelVolumeScroll(Q2DViewerBase + "enable2DViewerWheelVolumeScroll");
const QString CoreSettings::EnableQ2DViewerMouseWraparound(Q2DViewerBase + "enable2DViewerMouseWraparound");
const QString CoreSettings::EnableQ2DViewerReferenceLinesForMR(Q2DViewerBase + "enable2DViewerReferenceLinesForMR");
const QString CoreSettings::EnableQ2DViewerReferenceLinesForCT(Q2DViewerBase + "enable2DViewerReferenceLinesForCT");
const QString CoreSettings::ModalitiesWithZoomToolByDefault(Q2DViewerBase + "ModalitiesWithZoomToolByDefault");
const QString CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR(Q2DViewerBase + "enable2DViewerAutomaticSynchronizationForMR");
const QString CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT(Q2DViewerBase + "enable2DViewerAutomaticSynchronizationForCT");
const QString CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption(Q2DViewerBase + "modalitiesToApplyHangingProtocolsAsFirstOption");
const QString CoreSettings::StudyLayoutConfigList(Q2DViewerBase + "StudyLayoutConfigList");
const QString CoreSettings::ModalitiesWithPropagationEnabledByDefault(Q2DViewerBase + "ModalitiesWithPropagationEnabledByDefault");

const QString CoreSettings::MeasurementDisplayVerbosity(Q2DViewerBase + "Measurement/measurementDisplayVerbosity");
const QString CoreSettings::SUVMeasurementNormalizationType(Q2DViewerBase + "Measurement/SUVMeasurementNormalizationType");

const QString CoreSettings::EnabledSyncActionsKey("enabledSyncActions");

const QString CoreSettings::DontForceMultiSampling("DontForceMultiSampling");

const QString CoreSettings::ScaleFactor("scaleFactor");

const QString CoreSettings::CrosshairInnerDiameter("CrosshairInnerDiameter");
const QString CoreSettings::CrosshairOuterDiameter("CrosshairOuterDiameter");

const QString CoreSettings::ShowViewersTextualInformation("ShowViewersTextualInformation");

const QString CoreSettings::Q2DViewerAnnotations(Q2DViewerBase + "Annotations");

const QString CoreSettings::ComparisonModeDivision("ComparisionModeDivision");

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
#ifdef CORPORATE_VERSION
    settingsRegistry->addSetting(UpdateCheckUrlAdditionalParameters, "machineID,groupID");
#endif
    settingsRegistry->addSetting(MammographyAutoOrientationExceptions, (QStringList() << "BAV" << "BAG" << "estereot"));
    settingsRegistry->addSetting(AllowAsynchronousVolumeLoading, true);
    settingsRegistry->addSetting(MaximumNumberOfVolumesLoadingConcurrently, 3);
    settingsRegistry->addSetting(MaximumNumberOfVisibleVoiLutComboItems, 50);
    settingsRegistry->addSetting(EnableQ2DViewerSliceScrollLoop, false);
    settingsRegistry->addSetting(EnableQ2DViewerPhaseScrollLoop, false);
    settingsRegistry->addSetting(EnableQ2DViewerWheelVolumeScroll, false);
    settingsRegistry->addSetting(EnableQ2DViewerMouseWraparound, true);
    settingsRegistry->addSetting(EnableQ2DViewerReferenceLinesForMR, true);
    settingsRegistry->addSetting(EnableQ2DViewerReferenceLinesForCT, false);
    settingsRegistry->addSetting(ModalitiesWithZoomToolByDefault, "MG;CR;RF;OP;DX;MR");
    settingsRegistry->addSetting(EnableQ2DViewerAutomaticSynchronizationForMR, true);
    settingsRegistry->addSetting(EnableQ2DViewerAutomaticSynchronizationForCT, true);
    // TODO Encara falta determinar quines seran les modalitats en les que posarem per defecte aplicar sempre hanging protocols
    settingsRegistry->addSetting(ModalitiesToApplyHangingProtocolsAsFirstOption, "CR;CT;MG;MR;US");
    settingsRegistry->addSetting(ModalitiesWithPropagationEnabledByDefault, "CT;MR;PT");
    settingsRegistry->addSetting(SUVMeasurementNormalizationType, "BodyWeight");
    settingsRegistry->addSetting(DontForceMultiSampling, false);

    settingsRegistry->addSetting(ScaleFactor, "0");

    settingsRegistry->addSetting(CrosshairInnerDiameter, 10);
    settingsRegistry->addSetting(CrosshairOuterDiameter, 30);

    settingsRegistry->addSetting(ShowViewersTextualInformation, true);

    settingsRegistry->addListSetting(Q2DViewerAnnotations, Q2DViewerAnnotationsSettingsHelper::getSettingsDefaultValue());

    settingsRegistry->addSetting(ComparisonModeDivision, ComparisonModeDivisionStrings[0]);
}

CoreSettings::ComparisonModeDivisionType CoreSettings::comparisonModeDivisionTypeFromString(const QString &string)
{
    int index = ComparisonModeDivisionStrings.indexOf(string);
    return index >= 0 ? static_cast<ComparisonModeDivisionType>(index) : CurrentLeftPriorRight;
}

QString CoreSettings::comparisonModeDivisionTypeToString(ComparisonModeDivisionType division)
{
    return ComparisonModeDivisionStrings[division];
}

} // End namespace udg
