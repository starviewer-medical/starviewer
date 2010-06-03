/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolregistry.h"
#include "tool.h"
#include "qviewer.h"
#include "logging.h"
// tools registrades
#include "zoomtool.h"
#include "slicingtool.h"
#include "slicingkeyboardtool.h"
#include "windowleveltool.h"
#include "referencelinestool.h"
#include "translatetool.h"
#include "voxelinformationtool.h"
#include "seedtool.h"
#include "magictool.h"
#include "rotate3dtool.h"
#include "screenshottool.h"
#include "synchronizetool.h"
#include "windowlevelpresetstool.h"
#include "polylineroitool.h"
#include "polylinetemporalroitool.h"
#include "distancetool.h"
#include "erasertool.h"
#include "cursor3dtool.h"
#include "angletool.h"
#include "editortool.h"
#include "imageplaneprojectiontool.h"
#include "nonclosedangletool.h"
#include "clippingplanestool.h"
#include "transdifferencetool.h"
#include "linepathtool.h"

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

Tool *ToolRegistry::getTool( const QString &toolName, QViewer *viewer )
{
    Tool *tool = 0;
    if( toolName == "ZoomTool" )
    {
        tool = new ZoomTool( viewer );
    }
    else if( toolName == "SlicingTool" )
    {
        tool = new SlicingTool( viewer );
    }
    else if( toolName == "ReferenceLinesTool" )
    {
        tool = new ReferenceLinesTool( viewer );
    }
    else if( toolName == "TranslateTool" )
    {
        tool = new TranslateTool( viewer );
    }
    else if( toolName == "VoxelInformationTool" )
    {
        tool = new VoxelInformationTool( viewer );
    }
    else if( toolName == "WindowLevelTool" )
    {
        tool = new WindowLevelTool( viewer );
    }
    else if( toolName == "SeedTool" )
    {
        tool = new SeedTool( viewer );
    }
    else if( toolName == "MagicTool" )
    {
        tool = new MagicTool( viewer );
    }
    else if( toolName == "ScreenShotTool" )
    {
        tool = new ScreenShotTool( viewer );
    }
    else if( toolName == "Rotate3DTool" )
    {
        tool = new Rotate3DTool( viewer );
    }
    else if( toolName == "SynchronizeTool" )
    {
        tool = new SynchronizeTool( viewer );
    }
    else if( toolName == "WindowLevelPresetsTool" )
    {
        tool = new WindowLevelPresetsTool( viewer );
    }
    else if( toolName == "PolylineROITool" )
    {
        tool = new PolylineROITool( viewer );
    }
    else if( toolName == "PolylineTemporalROITool" )
    {
        tool = new PolylineTemporalROITool( viewer );
    }
    else if( toolName == "DistanceTool" )
    {
        tool = new DistanceTool( viewer );
    }
    else if( toolName == "SlicingKeyboardTool" )
    {
        tool = new SlicingKeyboardTool( viewer );
    }
    else if( toolName == "EraserTool" )
    {
        tool = new EraserTool( viewer );
    }
    else if( toolName == "Cursor3DTool" )
    {
        tool = new Cursor3DTool( viewer );
    }
    else if( toolName == "AngleTool" )
    {
        tool = new AngleTool( viewer );
    }
    else if( toolName == "EditorTool" )
    {
        tool = new EditorTool( viewer );
    }
    else if( toolName == "ImagePlaneProjectionTool" )
    {
        tool = new ImagePlaneProjectionTool( viewer );
    }
    else if( toolName == "NonClosedAngleTool" )
    {
        tool = new NonClosedAngleTool( viewer );
    }
    else if( toolName == "ClippingPlanesTool" )
    {
        tool = new ClippingPlanesTool( viewer );
    }
    else if( toolName == "TransDifferenceTool" )
    {
        tool = new TransDifferenceTool( viewer );
    }
    else if( toolName == "LinePathTool" )
    {
        tool = new LinePathTool( viewer );
    }
    else
    {
        DEBUG_LOG( toolName + "> Tool no registrada!");
    }
    return tool;
}

QAction *ToolRegistry::getToolAction( const QString &toolName )
{
    QAction *toolAction = new QAction( 0 );
    toolAction->setCheckable( true );
    QString statusTip;
    QString toolTip;

    if( toolName == "SlicingTool" )
    {
        toolAction->setText( tr("Scroll") );
        toolAction->setIcon( QIcon(":/images/slicing.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::SlicingTool ) );
        statusTip = tr("Enable/Disable scroll tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "WindowLevelTool" )
    {
        toolAction->setText( tr("WW/WL") );
        toolAction->setIcon( QIcon(":/images/windowLevel.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::WindowLevelTool ) );
        statusTip = tr("Enable/Disable Window Level tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "ZoomTool" )
    {
        toolAction->setText( tr("Zoom") );
        toolAction->setIcon( QIcon(":/images/zoom.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::ZoomTool ) );
        statusTip = tr("Enable/Disable Zoom tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "TranslateTool" )
    {
        toolAction->setText( tr("Pan") );
        toolAction->setIcon( QIcon(":/images/move.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::TranslateTool ) );
        statusTip = tr("Enable/Disable Move tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "SeedTool" )
    {
        toolAction->setText( tr("Seed") );
        toolAction->setIcon( QIcon(":/images/seed.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::SeedTool ) );
        statusTip = tr("Put Seed tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "MagicTool" )
    {
        toolAction->setText( tr("Magic") );
		//TODO: Find a better icon
        toolAction->setIcon( QIcon(":/images/repeat.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::MagicTool ) );
        statusTip = tr("Enable/Disable Magic tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "ScreenShotTool" )
    {
        toolAction->setText( tr("Screen Shot") );
        toolAction->setIcon( QIcon(":/images/photo.png") );
        statusTip = tr("Enable/Disable Screen shot tool");
        toolTip = tr("Export a viewer screenshot to an image file format");
    }
    else if( toolName == "Rotate3DTool" )
    {
        toolAction->setText( tr("3D Rotation") );
        toolAction->setIcon( QIcon(":/images/rotate3d.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::Rotate3DTool ) );
        statusTip = tr("Enable/Disable 3D Rotation tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "DistanceTool" )
    {
        toolAction->setText( tr("Distance") );
        toolAction->setIcon( QIcon(":/images/distance.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::DistanceTool ) );
        statusTip = tr("Enable/Disable Distances tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "ROITool" )
    {
        toolAction->setText( tr("ROI's") );
        toolAction->setIcon( QIcon(":/images/roi.png") );
        statusTip = tr("Enable/Disable ROI tool");
    }
    else if( toolName == "ReferenceLinesTool" )
    {
        toolAction->setText( tr("Ref.Lines") );
        toolAction->setIcon( QIcon(":/images/referenceLines.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::ReferenceLinesTool ) );
        statusTip = tr("Enable/Disable Reference Lines tool");
        toolTip = tr("Reference Lines");
    }
    else if( toolName == "VoxelInformationTool" )
    {
        toolAction->setText( tr("Voxel Information") );
        toolAction->setIcon( QIcon(":/images/voxelInformation.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::VoxelInformationTool ) );
        statusTip = tr("Enable voxel information over cursor");
        toolTip = toolAction->text();
    }
    else if( toolName == "SynchronizeTool" )
    {
        toolAction->setText( tr("Synchronize tool") );
        toolAction->setIcon( QIcon(":/images/synchronize.png") );
        statusTip = tr("Enable/Disable Synchronize tool");
        toolTip = tr("Synchronize");
    }
    else if( toolName == "WindowLevelPresetsTool" )
    {
        toolAction->setText( tr("Window Level Presets tool") );
        statusTip = tr("Enable/Disable Window Level Presets tool");
    }
    else if( toolName == "PolylineROITool" )
    {
        toolAction->setText( tr("ROI") );
        toolAction->setIcon( QIcon(":/images/polyline.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::ROITool ) );
        statusTip = tr("Enable/Disable Polyline ROI tool");
        toolTip = tr("ROI");
    }
    else if( toolName == "PolylineTemporalROITool" )
    {
        toolAction->setText( tr("ROI") );
        toolAction->setIcon( QIcon(":/images/polyline.png") );
        statusTip = tr("Enable/Disable Polyline ROI tool");
        toolTip = tr("Temporal ROI");
    }
    else if( toolName == "SlicingKeyboardTool" )
    {
        toolAction->setText( tr("Keyboard slicing tool") );
        statusTip = tr("Enable/Disable keyboard slicing tool");
    }
    else if( toolName == "EraserTool" )
    {
        toolAction->setText( tr("Erase") );
        toolAction->setIcon( QIcon(":/images/eraser2.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::EraserTool ) );
        statusTip = tr("Enable/Disable eraser tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "Cursor3DTool" )
    {
        toolAction->setText( tr("3D Cursor") );
        toolAction->setIcon( QIcon(":/images/mouse.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::Cursor3DTool ) );
        statusTip = tr("Enable/Disable Cursor 3D tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "AngleTool" )
    {
        toolAction->setText( tr("Angle") );
        toolAction->setIcon( QIcon(":/images/angle.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::AngleTool ) );
        statusTip = tr("Enable/Disable angle tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "EditorTool" )
    {
        toolAction->setText( tr("Editor") );
        toolAction->setIcon( QIcon(":/images/pencil.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::EditorTool ) );
        statusTip = tr("Enable/Disable editor tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "ImagePlaneProjectionTool" )
    {
        toolAction->setText( tr("Image Plane Projection") );
        toolAction->setIcon( QIcon(":/images/imagePlaneProjection.png") );
        toolAction->setStatusTip( tr("Enable/Disable image plane projection tool") );
    }
    else if( toolName == "NonClosedAngleTool" )
    {
        toolAction->setText( tr("Open Angle") );
        toolAction->setIcon( QIcon(":/images/nonClosedAngle.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::NonClosedAngleTool ) );
        statusTip = tr("Enable/Disable open-angle tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "ClippingPlanesTool" )
    {
        toolAction->setText( tr("Clipping Planes") );
        toolAction->setIcon( QIcon(":/images/clippingBox.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::ClippingPlanesTool ) );
        statusTip = tr("Enable/Disable the clipping planes tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "TransDifferenceTool" )
    {
        toolAction->setText( tr("Translation Difference") );
        toolAction->setIcon( QIcon(":/images/moveDifference.png") );
        toolAction->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::TransDifferenceTool ) );
        statusTip = tr("Enable/Disable the translation difference tool");
        toolTip = toolAction->text();
    }
    else if( toolName == "LinePathTool" )
    {
        toolAction->setText( tr("Line Path") );
        toolAction->setStatusTip( tr("Enable/Disable line path tool") );
    }
    else
    {
        DEBUG_LOG(toolName + "> Tool no registrada!");
    }

    QString shortcutString;
    if( !toolAction->shortcut().toString().isEmpty() )
        shortcutString = " (" + toolAction->shortcut().toString() + ")";
    toolAction->setStatusTip( statusTip + shortcutString );
    toolAction->setToolTip( toolTip + shortcutString );

    return toolAction;
}

QPair< QAction *, QString > ToolRegistry::getActionToolPair( const QString &actionToolName )
{
    QPair< QAction *, QString > pair;
    QAction *action = new QAction( 0 );
    QString slot;
    QString statusTip;
    QString toolTip;
    
    if( actionToolName == "RestoreActionTool" )
    {
        action->setText( tr("Restore") );
        action->setIcon( QIcon(":/images/restore.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::RestoreTool ) );
        statusTip = tr("Restore to initial state");
        toolTip = statusTip;

        slot = SLOT( restore() );
    }
    else if ( actionToolName == "ClearCurrentViewAnnotationsActionTool" )
    {
        action->setText( tr("All of current viewer") );
        action->setIcon ( QIcon(":/images/eraserViewer.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::ClearCurrentViewAnnotationsTool ) );
        statusTip = tr("Clear primitives from viewer");
        toolTip = statusTip;

        slot = SLOT( clearViewer() );
    }
    else if( actionToolName == "InvertWindowLevelActionTool" )
    {
        action->setText( tr("Invert WL") );
        action->setIcon( QIcon(":/images/invertWL.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::InvertWindowLevelTool ) );
        statusTip = tr("Invert window level");
        toolTip = statusTip;

        slot = SLOT( invertWindowLevel() );
    }
    else if( actionToolName == "AxialViewActionTool" )
    {
        action->setText( tr("Axial") );
        action->setIcon( QIcon(":/images/axial.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::AxialViewTool ) );
        statusTip = tr("Switch to Axial View");
        toolTip = statusTip;

        slot = SLOT( resetViewToAxial() );
    }
    else if( actionToolName == "SagitalViewActionTool" )
    {
        action->setText( tr("Sagital") );
        action->setIcon( QIcon(":/images/sagital.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::SagitalViewTool ) );
        statusTip = tr("Switch to Saggital View");
        toolTip = statusTip;

        slot = SLOT( resetViewToSagital() );
    }
    else if( actionToolName == "CoronalViewActionTool" )
    {
        action->setText( tr("Coronal") );
        action->setIcon( QIcon(":/images/coronal.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::CoronalViewTool ) );
        statusTip = tr("Switch to Coronal View");
        toolTip = statusTip;

        slot = SLOT( resetViewToCoronal() );
    }
    else if( actionToolName == "RotateClockWiseActionTool" )
    {
        action->setText( tr("Rotate") );
        action->setIcon( QIcon(":/images/rotateClockWise.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::RotateClockWiseTool ) );
        statusTip = tr("Rotate the image in clockwise direction");
        toolTip = tr("Clockwise rotation").toLatin1();

        slot = SLOT( rotateClockWise() );
    }
    else if( actionToolName == "RotateCounterClockWiseActionTool" )
    {
        action->setText( tr("Rotate Counter Clockwise") );
        action->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::RotateCounterClockWiseTool ) );
        statusTip = tr("Rotate the image in counter clockwise direction");
        toolTip = tr("Counter-clockwise rotation");

        slot = SLOT( rotateCounterClockWise() );
    }
    else if( actionToolName == "HorizontalFlipActionTool" )
    {
        action->setText( tr("Horizontal Flip") );
        action->setIcon( QIcon(":/images/flipHorizontal.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::HorizontalFlipTool ) );
        statusTip = tr("Flip the image horizontally");
        toolTip = action->text();

        slot = SLOT( horizontalFlip() );
    }
    else if( actionToolName == "VerticalFlipActionTool" )
    {
        action->setText( tr("Vertical Flip") );
        action->setIcon( QIcon(":/images/flipVertical.png") );
        action->setShortcuts( ShortcutManager::getShortcuts( Shortcuts::VerticalFlipTool ) );
        statusTip = tr("Flip the image vertically");
        toolTip = action->text();

        slot = SLOT( verticalFlip() );
    }
    else
    {
        DEBUG_LOG(actionToolName + "> Action Tool no registrada!");
    }

    QString shortcutString;
    if( !action->shortcut().toString().isEmpty() )
        shortcutString = " (" + action->shortcut().toString() + ")";
    action->setStatusTip( statusTip + shortcutString );
    action->setToolTip( toolTip + shortcutString );

    pair.first = action;
    pair.second = slot;
    return pair;
}

}
