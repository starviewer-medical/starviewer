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

#include "toolregistry.h"
#include "tool.h"
#include "qviewer.h"
#include "logging.h"
// Tools registrades
#include "zoomtool.h"
#include "zoomwheeltool.h"
#include "slicingwheeltool.h"
#include "slicingmousetool.h"
#include "slicingkeyboardtool.h"
#include "windowleveltool.h"
#include "windowlevellefttool.h"
#include "referencelinestool.h"
#include "translatetool.h"
#include "translatelefttool.h"
#include "voxelinformationtool.h"
#include "seedtool.h"
#include "magicroitool.h"
#include "rotate3dtool.h"
#include "screenshottool.h"
#include "synchronizetool.h"
#include "voilutpresetstool.h"
#include "polylineroitool.h"
#include "polylinetemporalroitool.h"
#include "distancetool.h"
#include "erasertool.h"
#include "cursor3dtool.h"
#include "angletool.h"
#include "editortool.h"
#include "nonclosedangletool.h"
#include "clippingplanestool.h"
#include "transdifferencetool.h"
#include "ellipticalroitool.h"
#include "automaticsynchronizationtool.h"
#include "magnifyingglasstool.h"
#include "circletool.h"
#include "perpendiculardistancetool.h"
#include "arrowtool.h"

#include "shortcutmanager.h"
#include "shortcuts.h"

#include <QAction>

namespace udg {

ToolRegistry::ToolRegistry(QObject *parent)
 : QObject(parent)
{
}

ToolRegistry::~ToolRegistry()
{
}

Tool* ToolRegistry::getTool(const QString &toolName, QViewer *viewer)
{
    Tool *tool = 0;
    if (toolName == "ZoomTool")
    {
        tool = new ZoomTool(viewer);
    }
    else if (toolName == "ZoomWheelTool")
    {
        tool = new ZoomWheelTool(viewer);
    }
    else if (toolName == "SlicingMouseTool")
    {
        tool = new SlicingMouseTool(viewer);
    }
    else if (toolName == "SlicingWheelTool")
    {
        tool = new SlicingWheelTool(viewer);
    }
    else if (toolName == "ReferenceLinesTool")
    {
        tool = new ReferenceLinesTool(viewer);
    }
    else if (toolName == "TranslateTool")
    {
        tool = new TranslateTool(viewer);
    }
    else if (toolName == "TranslateLeftTool")
    {
        tool = new TranslateLeftTool(viewer);
    }
    else if (toolName == "VoxelInformationTool")
    {
        tool = new VoxelInformationTool(viewer);
    }
    else if (toolName == "WindowLevelTool")
    {
        tool = new WindowLevelTool(viewer);
    }
    else if (toolName == "WindowLevelLeftTool")
    {
        tool = new WindowLevelLeftTool(viewer);
    }
    else if (toolName == "SeedTool")
    {
        tool = new SeedTool(viewer);
    }
    else if (toolName == "MagicROITool")
    {
        tool = new MagicROITool(viewer);
    }
    else if (toolName == "ScreenShotTool")
    {
        tool = new ScreenShotTool(viewer);
    }
    else if (toolName == "Rotate3DTool")
    {
        tool = new Rotate3DTool(viewer);
    }
    else if (toolName == "SynchronizeTool")
    {
        tool = new SynchronizeTool(viewer);
    }
    else if (toolName == "VoiLutPresetsTool")
    {
        tool = new VoiLutPresetsTool(viewer);
    }
    else if (toolName == "PolylineROITool")
    {
        tool = new PolylineROITool(viewer);
    }
    else if (toolName == "PolylineTemporalROITool")
    {
        tool = new PolylineTemporalROITool(viewer);
    }
    else if (toolName == "DistanceTool")
    {
        tool = new DistanceTool(viewer);
    }
    else if (toolName == "SlicingKeyboardTool")
    {
        tool = new SlicingKeyboardTool(viewer);
    }
    else if (toolName == "EraserTool")
    {
        tool = new EraserTool(viewer);
    }
    else if (toolName == "Cursor3DTool")
    {
        tool = new Cursor3DTool(viewer);
    }
    else if (toolName == "AngleTool")
    {
        tool = new AngleTool(viewer);
    }
    else if (toolName == "EditorTool")
    {
        tool = new EditorTool(viewer);
    }
    else if (toolName == "NonClosedAngleTool")
    {
        tool = new NonClosedAngleTool(viewer);
    }
    else if (toolName == "ClippingPlanesTool")
    {
        tool = new ClippingPlanesTool(viewer);
    }
    else if (toolName == "TransDifferenceTool")
    {
        tool = new TransDifferenceTool(viewer);
    }
    else if (toolName == "EllipticalROITool")
    {
        tool = new EllipticalROITool(viewer);
    }
    else if (toolName == "AutomaticSynchronizationTool")
    {
        tool = new AutomaticSynchronizationTool(viewer);
    }
    else if (toolName == "MagnifyingGlassTool")
    {
        tool = new MagnifyingGlassTool(viewer);
    }
    else if (toolName == "CircleTool")
    {
        tool = new CircleTool(viewer);
    }
    else if (toolName == "PerpendicularDistanceTool")
    {
        tool = new PerpendicularDistanceTool(viewer);
    }
    else if (toolName == "ArrowTool")
    {
        tool = new ArrowTool(viewer);
    }
    else
    {
        DEBUG_LOG(toolName + "> Tool no registrada!");
    }
    return tool;
}

QAction* ToolRegistry::getToolAction(const QString &toolName)
{
    QAction *toolAction = new QAction(this);
    toolAction->setCheckable(true);
    QString statusTip;
    QString toolTip;

    if (toolName == "SlicingMouseTool")
    {
        toolAction->setText(tr("Scroll"));
        toolAction->setIcon(QIcon(":/images/icons/layer-previous.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SlicingMouseTool));
        statusTip = tr("Enable/Disable scroll tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "SlicingWheelTool")
    {
        toolAction->setText(tr("Scroll"));
        toolAction->setIcon(QIcon(":/images/icons/layer-previous.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SlicingWheelTool));
        statusTip = tr("Enable/Disable scroll wheel tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "WindowLevelTool")
    {
        toolAction->setText(tr("WW/WL"));
        toolAction->setIcon(QIcon(":/images/icons/contrast.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::WindowLevelTool));
        statusTip = tr("Enable/Disable Window Level tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "WindowLevelLeftTool")
    {
        toolAction->setText(tr("WW/WL"));
        toolAction->setIcon(QIcon(":/images/icons/contrast.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::WindowLevelTool));
        statusTip = tr("Enable/Disable Window Level tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ZoomTool")
    {
        toolAction->setText(tr("Zoom"));
        toolAction->setIcon(QIcon(":/images/icons/edit-find.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ZoomTool));
        statusTip = tr("Enable/Disable Zoom tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ZoomWheelTool")
    {
        toolAction->setText(tr("Zoom"));
        toolAction->setIcon(QIcon(":/images/icons/edit-find.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ZoomWheelTool));
        statusTip = tr("Enable/Disable Zoom Wheel tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "TranslateTool")
    {
        toolAction->setText(tr("Pan"));
        toolAction->setIcon(QIcon(":/images/icons/transform-move.svg"));
        statusTip = tr("Enable/Disable Move tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "TranslateLeftTool")
    {
        toolAction->setText(tr("Pan"));
        toolAction->setIcon(QIcon(":/images/icons/transform-move.svg"));
        statusTip = tr("Enable/Disable Move tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "SeedTool")
    {
        toolAction->setText(tr("Seed"));
        toolAction->setIcon(QIcon(":/images/icons/seed.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SeedTool));
        statusTip = tr("Put Seed tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "MagicROITool")
    {
        toolAction->setText(tr("Magical ROI"));
        toolAction->setIcon(QIcon(":/images/icons/tools-wizard.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MagicROITool));
        statusTip = tr("Enable/Disable Magic tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ScreenShotTool")
    {
        toolAction->setText(tr("Screenshot"));
        toolAction->setIcon(QIcon(":/images/icons/camera-photo.svg"));
        statusTip = tr("Enable/Disable Screenshot tool");
        toolTip = tr("Export a viewer screenshot to an image file format");
    }
    else if (toolName == "Rotate3DTool")
    {
        toolAction->setText(tr("3D Rotation"));
        toolAction->setIcon(QIcon(":/images/icons/swivel.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::Rotate3DTool));
        statusTip = tr("Enable/Disable 3D Rotation tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "DistanceTool")
    {
        toolAction->setText(tr("Distance"));
        toolAction->setIcon(QIcon(":/images/icons/kruler-west.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::DistanceTool));
        statusTip = tr("Enable/Disable Distances tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ROITool")
    {
        toolAction->setText(tr("ROIs"));
        toolAction->setIcon(QIcon(":/images/icons/roi.svg"));
        statusTip = tr("Enable/Disable ROI tool");
    }
    else if (toolName == "ReferenceLinesTool")
    {
        toolAction->setText(tr("Ref. Lines"));
        toolAction->setIcon(QIcon(":/images/icons/reference-lines.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ReferenceLinesTool));
        statusTip = tr("Enable/Disable Reference Lines tool");
        toolTip = tr("Reference Lines");
    }
    else if (toolName == "VoxelInformationTool")
    {
        toolAction->setText(tr("Voxel Information"));
        toolAction->setIcon(QIcon(":/images/icons/edit-select.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::VoxelInformationTool));
        statusTip = tr("Enable voxel information under cursor");
        toolTip = toolAction->text();
    }
    else if (toolName == "SynchronizeTool")
    {
        toolAction->setText(tr("Synchronize tool"));
        toolAction->setIcon(QIcon(":/images/icons/emblem-symbolic-link.svg"));
        statusTip = tr("Enable/Disable Synchronize tool");
        toolTip = tr("Synchronize");
    }
    else if (toolName == "VoiLutPresetsTool")
    {
        toolAction->setText(tr("VOI LUT Presets tool"));
        statusTip = tr("Enable/Disable VOI LUT Presets tool");
    }
    else if (toolName == "PolylineROITool")
    {
        toolAction->setText(tr("Polyline ROI"));
        toolAction->setIcon(QIcon(":/images/icons/draw-polyline.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ROITool));
        statusTip = tr("Enable/Disable Polyline ROI tool");
        toolTip = tr("ROI");
    }
    else if (toolName == "PolylineTemporalROITool")
    {
        toolAction->setText(tr("ROI"));
        toolAction->setIcon(QIcon(":/images/icons/draw-polyline.svg"));
        statusTip = tr("Enable/Disable Polyline ROI tool");
        toolTip = tr("Temporal ROI");
    }
    else if (toolName == "SlicingKeyboardTool")
    {
        toolAction->setText(tr("Keyboard slicing tool"));
        statusTip = tr("Enable/Disable keyboard slicing tool");
    }
    else if (toolName == "EraserTool")
    {
        toolAction->setText(tr("Erase"));
        toolAction->setIcon(QIcon(":/images/icons/draw-eraser.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::EraserTool));
        statusTip = tr("Enable/Disable eraser tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "Cursor3DTool")
    {
        toolAction->setText(tr("3D Cursor"));
        toolAction->setIcon(QIcon(":/images/icons/crosshairs.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::Cursor3DTool));
        statusTip = tr("Enable/Disable Cursor 3D tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "AngleTool")
    {
        toolAction->setText(tr("Angle"));
        toolAction->setIcon(QIcon(":/images/icons/measure-angle.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::AngleTool));
        statusTip = tr("Enable/Disable angle tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "EditorTool")
    {
        toolAction->setText(tr("Editor"));
        toolAction->setIcon(QIcon(":/images/icons/pencil.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::EditorTool));
        statusTip = tr("Enable/Disable editor tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "NonClosedAngleTool")
    {
        toolAction->setText(tr("Cobb angle"));
        toolAction->setIcon(QIcon(":/images/icons/measure-open-angle.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::NonClosedAngleTool));
        statusTip = tr("Enable/Disable open-angle tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ClippingPlanesTool")
    {
        toolAction->setText(tr("Clipping Planes"));
        toolAction->setIcon(QIcon(":/images/icons/node.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ClippingPlanesTool));
        statusTip = tr("Enable/Disable the clipping planes tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "TransDifferenceTool")
    {
        toolAction->setText(tr("Translation Difference"));
        toolAction->setIcon(QIcon(":/images/icons/transform-move-difference.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::TransDifferenceTool));
        statusTip = tr("Enable/Disable the translation difference tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "EllipticalROITool")
    {
        toolAction->setText(tr("Elliptical ROI"));
        toolAction->setIcon(QIcon(":/images/icons/draw-ellipse.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::EllipticalROITool));
        statusTip = tr("Enable/Disable Elliptical ROI tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "AutomaticSynchronizationTool")
    {
        toolAction->setText(tr("Auto-Sync"));
        toolAction->setIcon(QIcon(":/images/icons/emblem-symbolic-link.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::AutomaticSynchronizationTool));
        statusTip = tr("Enable/Disable Automatic synchronization tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "MagnifyingGlassTool")
    {
        toolAction->setText(tr("Magnify"));
        toolAction->setIcon(QIcon(":/images/icons/magnifying-glass.svg"));
        toolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MagnifyingGlassTool));
        statusTip = tr("Enable/Disable the magnifying glass tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "CircleTool")
    {
        toolAction->setText(tr("Circle"));
        toolAction->setIcon(QIcon(":/images/icons/draw-circle.svg"));
        statusTip = tr("Enable/Disable Circle tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "PerpendicularDistanceTool")
    {
        toolAction->setText(tr("TA-GT"));
        toolAction->setIcon(QIcon(":/images/icons/ta-gt.svg"));
        statusTip = tr("Enable/Disable TA-GT distance tool");
        toolTip = toolAction->text();
    }
    else if (toolName == "ArrowTool")
    {
        toolAction->setText(tr("Arrow"));
        toolAction->setIcon(QIcon(":/images/icons/draw-arrow.svg"));
        statusTip = tr("Enable/disable arrow tool");
        toolTip = toolAction->text();
    }
    else
    {
        DEBUG_LOG(toolName + "> Tool no registrada!");
    }

    QString shortcutString;
    if (!toolAction->shortcut().toString().isEmpty())
    {
        shortcutString = " (" + toolAction->shortcut().toString() + ")";
    }
    toolAction->setStatusTip(statusTip + shortcutString);
    toolAction->setToolTip(toolTip + shortcutString);

    return toolAction;
}

QPair<QAction*, QString> ToolRegistry::getActionToolPair(const QString &actionToolName)
{
    QPair<QAction*, QString> pair;
    QAction *action = new QAction(this);
    QString slot;
    QString statusTip;
    QString toolTip;

    if (actionToolName == "RestoreActionTool")
    {
        action->setText(tr("Restore"));
        action->setIcon(QIcon(":/images/icons/reset-view.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::RestoreTool));
        statusTip = tr("Restore to initial state");
        toolTip = statusTip;

        slot = SLOT(restore());
    }
    else if (actionToolName == "ClearCurrentViewAnnotationsActionTool")
    {
        action->setText(tr("All of current viewer"));
        action->setIcon (QIcon(":/images/icons/draw-erase-all.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ClearCurrentViewAnnotationsTool));
        statusTip = tr("Clear primitives from viewer");
        toolTip = statusTip;

        slot = SLOT(clearViewer());
    }
    else if (actionToolName == "InvertVoiLutActionTool")
    {
        action->setText(tr("Invert VOI LUT"));
        action->setIcon(QIcon(":/images/icons/edit-select-invert.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::InvertVoiLutTool));
        statusTip = tr("Invert VOI LUT");
        toolTip = statusTip;

        slot = SLOT(invertVoiLut());
    }
    else if (actionToolName == "AxialViewActionTool")
    {
        action->setText(tr("Axial"));
        action->setIcon(QIcon(":/images/icons/axial.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::AxialViewTool));
        statusTip = tr("Switch to Axial View");
        toolTip = statusTip;

        slot = SLOT(resetViewToAxial());
    }
    else if (actionToolName == "SagitalViewActionTool")
    {
        action->setText(tr("Sagittal"));
        action->setIcon(QIcon(":/images/icons/sagittal.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SagitalViewTool));
        statusTip = tr("Switch to Sagittal View");
        toolTip = statusTip;

        slot = SLOT(resetViewToSagital());
    }
    else if (actionToolName == "CoronalViewActionTool")
    {
        action->setText(tr("Coronal"));
        action->setIcon(QIcon(":/images/icons/coronal.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::CoronalViewTool));
        statusTip = tr("Switch to Coronal View");
        toolTip = statusTip;

        slot = SLOT(resetViewToCoronal());
    }
    else if (actionToolName == "RotateClockWiseActionTool")
    {
        action->setText(tr("Rotate"));
        action->setIcon(QIcon(":/images/icons/object-rotate-right.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::RotateClockWiseTool));
        statusTip = tr("Rotate the image in clockwise direction");
        toolTip = tr("Clockwise rotation");

        slot = SLOT(rotateClockWise());
    }
    else if (actionToolName == "RotateCounterClockWiseActionTool")
    {
        action->setText(tr("Rotate Counter Clockwise"));
        action->setIcon(QIcon(":/images/icons/object-rotate-left.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::RotateCounterClockWiseTool));
        statusTip = tr("Rotate the image in counter clockwise direction");
        toolTip = tr("Counter-clockwise rotation");

        slot = SLOT(rotateCounterClockWise());
    }
    else if (actionToolName == "HorizontalFlipActionTool")
    {
        action->setText(tr("Horizontal Flip"));
        action->setIcon(QIcon(":/images/icons/object-flip-horizontal.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::HorizontalFlipTool));
        statusTip = tr("Flip the image horizontally");
        toolTip = action->text();

        slot = SLOT(horizontalFlip());
    }
    else if (actionToolName == "VerticalFlipActionTool")
    {
        action->setText(tr("Vertical Flip"));
        action->setIcon(QIcon(":/images/icons/object-flip-vertical.svg"));
        action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::VerticalFlipTool));
        statusTip = tr("Flip the image vertically");
        toolTip = action->text();

        slot = SLOT(verticalFlip());
    }
    else
    {
        DEBUG_LOG(actionToolName + "> Action Tool no registrada!");
    }

    QString shortcutString;
    if (!action->shortcut().toString().isEmpty())
    {
        shortcutString = " (" + action->shortcut().toString() + ")";
    }
    action->setStatusTip(statusTip + shortcutString);
    action->setToolTip(toolTip + shortcutString);

    pair.first = action;
    pair.second = slot;
    return pair;
}

}
