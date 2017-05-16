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

#include "shortcuts.h"
#include "settingsregistry.h"
#include <QList>
#include <QKeySequence>

namespace udg {

// Definició de les claus
const QString ShortcutsBase("Shortcuts/");
const QString Shortcuts::SlicingMouseTool(ShortcutsBase + "SlicingMouseTool");
const QString Shortcuts::SlicingWheelTool(ShortcutsBase + "SlicingWheelTool");
const QString Shortcuts::WindowLevelTool(ShortcutsBase + "WindowLevelTool");
const QString Shortcuts::ZoomTool(ShortcutsBase + "ZoomTool");
const QString Shortcuts::SeedTool(ShortcutsBase + "SeedTool");
const QString Shortcuts::MagicROITool(ShortcutsBase + "MagicROITool");
const QString Shortcuts::ScreenShotTool(ShortcutsBase + "ScreenShotTool");
const QString Shortcuts::Rotate3DTool(ShortcutsBase + "Rotate3DTool");
const QString Shortcuts::DistanceTool(ShortcutsBase + "DistanceTool");
const QString Shortcuts::ROITool(ShortcutsBase + "ROITool");
const QString Shortcuts::ReferenceLinesTool(ShortcutsBase + "ReferenceLinesTool");
const QString Shortcuts::VoxelInformationTool(ShortcutsBase + "VoxelInformationTool");
const QString Shortcuts::SynchronizeTool(ShortcutsBase + "SynchronizeTool");
const QString Shortcuts::EraserTool(ShortcutsBase + "EraserTool");
const QString Shortcuts::Cursor3DTool(ShortcutsBase + "Cursor3DTool");
const QString Shortcuts::AngleTool(ShortcutsBase + "AngleTool");
const QString Shortcuts::NonClosedAngleTool(ShortcutsBase + "NonClosedAngleTool");
const QString Shortcuts::EditorTool(ShortcutsBase + "EditorTool");
const QString Shortcuts::ClippingPlanesTool(ShortcutsBase + "ClippingPlanesTool");
const QString Shortcuts::RestoreTool(ShortcutsBase + "RestoreTool");
const QString Shortcuts::InvertVoiLutTool(ShortcutsBase + "InvertVoiLutTool");
const QString Shortcuts::AxialViewTool(ShortcutsBase + "AxialViewTool");
const QString Shortcuts::SagitalViewTool(ShortcutsBase + "SagitalViewTool");
const QString Shortcuts::CoronalViewTool(ShortcutsBase + "CoronalViewTool");
const QString Shortcuts::RotateClockWiseTool(ShortcutsBase + "RotateClockWiseTool");
const QString Shortcuts::RotateCounterClockWiseTool(ShortcutsBase + "RotateCounterClockWiseTool");
const QString Shortcuts::HorizontalFlipTool(ShortcutsBase + "HorizontalFlipTool");
const QString Shortcuts::VerticalFlipTool(ShortcutsBase + "VerticalFlipTool");
const QString Shortcuts::ClearCurrentViewAnnotationsTool(ShortcutsBase + "ClearCurrentViewAnnotationsActionTool");
const QString Shortcuts::TransDifferenceTool(ShortcutsBase + "TransDifferenceActionTool");
const QString Shortcuts::EllipticalROITool(ShortcutsBase + "EllipticalROITool");
const QString Shortcuts::MagnifyingGlassTool(ShortcutsBase + "MagnifyingGlassTool");
const QString Shortcuts::AutomaticSynchronizationTool(ShortcutsBase + "AutomaticSynchronizationTool");
const QString Shortcuts::SynchronizeAllViewers(ShortcutsBase + "SynchronizeAllViewers");
const QString Shortcuts::DesynchronizeAllViewers(ShortcutsBase + "SynchronizeNoneViewer");
const QString Shortcuts::Propagation(ShortcutsBase + "Propagation");
const QString Shortcuts::NextHangingProtocol(ShortcutsBase + "NextHangingProtocol");
const QString Shortcuts::PreviousHangingProtocol(ShortcutsBase + "PreviousHangingProtocol");
const QString Shortcuts::ToggleComparativeStudiesMode(ShortcutsBase + "ToggleComparativeStudiesMode");

const QString Shortcuts::SaveSingleScreenShot(ShortcutsBase + "SaveSingleScreenShot");
const QString Shortcuts::SaveWholeSeriesScreenShot(ShortcutsBase + "SaveWholeSeriesScreenShot");

const QString Shortcuts::NewWindow(ShortcutsBase + "NewWindow");
const QString Shortcuts::OpenFile(ShortcutsBase + "OpenFile");
const QString Shortcuts::OpenDICOMDIR(ShortcutsBase + "OpenDICOMDIR");
const QString Shortcuts::OpenDirectory(ShortcutsBase + "OpenDirectory");
const QString Shortcuts::OpenLocalDatabaseStudies(ShortcutsBase + "OpenLocalDatabaseStudies");
const QString Shortcuts::OpenPACS(ShortcutsBase + "OpenPACS");
const QString Shortcuts::OpenExams(ShortcutsBase + "OpenExams");
const QString Shortcuts::Preferences(ShortcutsBase + "Preferences");
const QString Shortcuts::CloseCurrentExtension(ShortcutsBase + "CloseViewer");
const QString Shortcuts::CloseApplication(ShortcutsBase + "CloseApp");

const QString Shortcuts::ViewSelectedStudies(ShortcutsBase + "ViewSelectedStudies");
const QString Shortcuts::ImportToLocalDatabaseSelectedDICOMDIRStudies(ShortcutsBase + "ImportToLocalDatabaseSelectedDICOMDIRStudies");
const QString Shortcuts::DeleteSelectedLocalDatabaseStudies(ShortcutsBase + "DeleteSelectedLocalDatabaseStudies");
const QString Shortcuts::StoreSelectedStudiesToPACS(ShortcutsBase + "StoreSelectedStudiesToPACS");
const QString Shortcuts::SendSelectedStudiesToDICOMDIRList(ShortcutsBase + "SendSelectedStudiesToDICOMDIRList");
const QString Shortcuts::RetrieveAndViewSelectedStudies(ShortcutsBase + "RetrieveAndViewSelectedStudies");
const QString Shortcuts::RetrieveSelectedStudies(ShortcutsBase + "RetrieveSelectedStudies");

const QString Shortcuts::MoveToPreviousDesktop(ShortcutsBase + "MoveToPreviousDesktop");
const QString Shortcuts::MoveToNextDesktop(ShortcutsBase + "MoveToNextDesktop");
const QString Shortcuts::MaximizeMultipleScreens(ShortcutsBase + "MaximizeMultipleScreens");

const QString Shortcuts::ExternalApplication1(ShortcutsBase + "ExternalApplication1");
const QString Shortcuts::ExternalApplication2(ShortcutsBase + "ExternalApplication2");
const QString Shortcuts::ExternalApplication3(ShortcutsBase + "ExternalApplication3");
const QString Shortcuts::ExternalApplication4(ShortcutsBase + "ExternalApplication4");
const QString Shortcuts::ExternalApplication5(ShortcutsBase + "ExternalApplication5");
const QString Shortcuts::ExternalApplication6(ShortcutsBase + "ExternalApplication6");
const QString Shortcuts::ExternalApplication7(ShortcutsBase + "ExternalApplication7");
const QString Shortcuts::ExternalApplication8(ShortcutsBase + "ExternalApplication8");
const QString Shortcuts::ExternalApplication9(ShortcutsBase + "ExternalApplication9");
const QString Shortcuts::ExternalApplication10(ShortcutsBase + "ExternalApplication10");
const QString Shortcuts::ExternalApplication11(ShortcutsBase + "ExternalApplication11");
const QString Shortcuts::ExternalApplication12(ShortcutsBase + "ExternalApplication12");

Shortcuts::Shortcuts()
{
}

Shortcuts::~Shortcuts()
{
}

void Shortcuts::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();

    QList<QVariant> shortcutsList;
    shortcutsList.append(QString("S"));
    settingsRegistry->addSetting(SlicingMouseTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("S"));
    settingsRegistry->addSetting(SlicingWheelTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("W"));
    settingsRegistry->addSetting(WindowLevelTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Z"));
    settingsRegistry->addSetting(ZoomTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("X"));
    settingsRegistry->addSetting(SeedTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("M"));
    settingsRegistry->addSetting(MagicROITool, shortcutsList);

    shortcutsList.clear();
    // \TODO
    shortcutsList.append(QString(""));
    settingsRegistry->addSetting(ScreenShotTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Q"));
    settingsRegistry->addSetting(Rotate3DTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("D"));
    settingsRegistry->addSetting(DistanceTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+R"));
    settingsRegistry->addSetting(ROITool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("L"));
    settingsRegistry->addSetting(ReferenceLinesTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("V"));
    settingsRegistry->addSetting(VoxelInformationTool, shortcutsList);

    shortcutsList.clear();
    // \TODO
    shortcutsList.append(QString(""));
    settingsRegistry->addSetting(SynchronizeTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("E"));
    settingsRegistry->addSetting(EraserTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("C"));
    settingsRegistry->addSetting(Cursor3DTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("A"));
    settingsRegistry->addSetting(AngleTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+A"));
    settingsRegistry->addSetting(NonClosedAngleTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("N"));
    settingsRegistry->addSetting(EditorTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("B"));
    settingsRegistry->addSetting(ClippingPlanesTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+R"));
    settingsRegistry->addSetting(RestoreTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("I"));
    settingsRegistry->addSetting(InvertVoiLutTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+1"));
    settingsRegistry->addSetting(AxialViewTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+2"));
    settingsRegistry->addSetting(SagitalViewTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+3"));
    settingsRegistry->addSetting(CoronalViewTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("G"));
    settingsRegistry->addSetting(RotateClockWiseTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("F"));
    settingsRegistry->addSetting(RotateCounterClockWiseTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("H"));
    settingsRegistry->addSetting(HorizontalFlipTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("J"));
    settingsRegistry->addSetting(VerticalFlipTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+E"));
    settingsRegistry->addSetting(ClearCurrentViewAnnotationsTool, shortcutsList);

    shortcutsList.clear();
    // \TODO
    shortcutsList.append(QString(""));
    settingsRegistry->addSetting(TransDifferenceTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("R"));
    settingsRegistry->addSetting(EllipticalROITool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+S"));
    settingsRegistry->addSetting(SaveSingleScreenShot, shortcutsList);
    
    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+A"));
    settingsRegistry->addSetting(SaveWholeSeriesScreenShot, shortcutsList);
    
    shortcutsList.clear();
    shortcutsList.append(QKeySequence(QKeySequence::New).toString());
    settingsRegistry->addSetting(NewWindow, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QKeySequence(QKeySequence::Open).toString());
    settingsRegistry->addSetting(OpenFile, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+D"));
    settingsRegistry->addSetting(OpenDICOMDIR, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+Shift+D"));
    settingsRegistry->addSetting(OpenDirectory, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+L"));
    settingsRegistry->addSetting(OpenLocalDatabaseStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+P"));
    settingsRegistry->addSetting(OpenPACS, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+E"));
    settingsRegistry->addSetting(OpenExams, shortcutsList);

    shortcutsList.clear();
    // \TODO
    shortcutsList.append(QString(""));
    settingsRegistry->addSetting(Preferences, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+W"));
    settingsRegistry->addSetting(CloseCurrentExtension, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+Q"));
    settingsRegistry->addSetting(CloseApplication, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+V"));
    settingsRegistry->addSetting(ViewSelectedStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+R"));
    settingsRegistry->addSetting(ImportToLocalDatabaseSelectedDICOMDIRStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QKeySequence(Qt::Key_Delete).toString());
    settingsRegistry->addSetting(DeleteSelectedLocalDatabaseStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+S"));
    settingsRegistry->addSetting(StoreSelectedStudiesToPACS, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+M"));
    settingsRegistry->addSetting(SendSelectedStudiesToDICOMDIRList, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+V"));
    settingsRegistry->addSetting(RetrieveAndViewSelectedStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+R"));
    settingsRegistry->addSetting(RetrieveSelectedStudies, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+Shift+Left"));
    settingsRegistry->addSetting(MoveToPreviousDesktop, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+Shift+Right"));
    settingsRegistry->addSetting(MoveToNextDesktop, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Ctrl+Shift+M"));
    settingsRegistry->addSetting(MaximizeMultipleScreens, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+Z"));
    settingsRegistry->addSetting(MagnifyingGlassTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("O"));
    settingsRegistry->addSetting(AutomaticSynchronizationTool, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("U"));
    settingsRegistry->addSetting(SynchronizeAllViewers, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+U"));
    settingsRegistry->addSetting(DesynchronizeAllViewers, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("P"));
    settingsRegistry->addSetting(Propagation, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("F12"));
    settingsRegistry->addSetting(NextHangingProtocol, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("F11"));
    settingsRegistry->addSetting(PreviousHangingProtocol, shortcutsList);
    
    shortcutsList.clear();
    shortcutsList.append(QString("F10"));
    settingsRegistry->addSetting(ToggleComparativeStudiesMode, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F1"));
    settingsRegistry->addSetting(ExternalApplication1, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F2"));
    settingsRegistry->addSetting(ExternalApplication2, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F3"));
    settingsRegistry->addSetting(ExternalApplication3, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F4"));
    settingsRegistry->addSetting(ExternalApplication4, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F5"));
    settingsRegistry->addSetting(ExternalApplication5, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F6"));
    settingsRegistry->addSetting(ExternalApplication6, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F7"));
    settingsRegistry->addSetting(ExternalApplication7, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F8"));
    settingsRegistry->addSetting(ExternalApplication8, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F9"));
    settingsRegistry->addSetting(ExternalApplication9, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F10"));
    settingsRegistry->addSetting(ExternalApplication10, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F11"));
    settingsRegistry->addSetting(ExternalApplication11, shortcutsList);

    shortcutsList.clear();
    shortcutsList.append(QString("Shift+F12"));
    settingsRegistry->addSetting(ExternalApplication12, shortcutsList);

}

} // End namespace udg
