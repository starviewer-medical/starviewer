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
#include "windowleveltool.h"
#include "referencelinestool.h"
#include "translatetool.h"
#include "voxelinformationtool.h"
#include "seedtool.h"
#include "rotate3dtool.h"
#include "screenshottool.h"
#include "synchronizetool.h"
#include "windowlevelpresetstool.h"
#include "polylineroitool.h"
#include "distancetool.h"

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
    else if( toolName == "DistanceTool" )
    {
        tool = new DistanceTool( viewer );
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
    if( toolName == "SlicingTool" )
    {
        toolAction->setText( tr("Slicer") );
        toolAction->setStatusTip( tr("Enable/Disable slicing tool") );
        toolAction->setIcon( QIcon(":/images/slicing.png") );
    }
    else if( toolName == "WindowLevelTool" )
    {
        toolAction->setText( tr("Window Level") );
        toolAction->setStatusTip( tr("Enable/Disable Window Level tool") );
        toolAction->setIcon( QIcon(":/images/windowLevel.png") );
    }
    else if( toolName == "ZoomTool" )
    {
        toolAction->setText( tr("Zoom") );
        toolAction->setStatusTip( tr("Enable/Disable Zoom tool") );
        toolAction->setIcon( QIcon(":/images/zoom.png") );
    }
    else if( toolName == "TranslateTool" )
    {
        toolAction->setText( tr("Move") );
        toolAction->setStatusTip( tr("Enable/Disable Move tool") );
        toolAction->setIcon( QIcon(":/images/move.png") );
    }
    else if( toolName == "SeedTool" )
    {
        toolAction->setText( tr("Seed") );
        toolAction->setStatusTip( tr("Put Seed tool") );
        toolAction->setIcon( QIcon(":/images/seed.png") );
    }
    else if( toolName == "ScreenShotTool" )
    {
        toolAction->setText( tr("Screen Shot") );
        toolAction->setStatusTip( tr("Enable/Disable Screen shot tool") );
        toolAction->setIcon( QIcon(":/images/photo.png") );
    }
    else if( toolName == "Rotate3DTool" )
    {
        toolAction->setText( tr("3D Rotation") );
        toolAction->setStatusTip( tr("Enable/Disable 3D Rotation tool") );
        toolAction->setIcon( QIcon(":/images/rotate3d.png") );
    }
    else if( toolName == "DistanceTool" )
    {
        toolAction->setText( tr("Distances") );
        toolAction->setStatusTip( tr("Enable/Disable Distances tool") );
        toolAction->setIcon( QIcon(":/images/distance.png") );
    }
    else if( toolName == "ROITool" )
    {
        toolAction->setText( tr("ROI's") );
        toolAction->setStatusTip( tr("Enable/Disable ROI tool") );
        toolAction->setIcon( QIcon(":/images/roi.png") );
    }
    else if( toolName == "ReferenceLinesTool" )
    {
        toolAction->setText( tr("Reference Lines") );
        toolAction->setStatusTip( tr("Enable/Disable Reference Lines tool") );
//         toolAction->setIcon( QIcon(":/images/roi.png") ); TODO icona per determinar
    }
    else if( toolName == "VoxelInformationTool" )
    {
        toolAction->setText( tr("Voxel Information") );
        toolAction->setStatusTip( tr("Enable voxel information over cursor") );
        toolAction->setIcon( QIcon(":/images/voxelInformation.png") );
    }
    else if( toolName == "SynchronizeTool" )
    {
        toolAction->setText( tr("Synchronize tool") );
        toolAction->setStatusTip( tr("Enable/Disable Synchronize tool") );
        toolAction->setIcon( QIcon(":/images/synchronize.png") );
    }
    else if( toolName == "WindowLevelPresetsTool" )
    {
        toolAction->setText( tr("Window Level Presets tool") );
        toolAction->setStatusTip( tr("Enable/Disable Window Level Presets tool") );
    }
    else if( toolName == "PolylineROITool" )
    {
        toolAction->setText( tr("Polyline ROI tool") );
        toolAction->setStatusTip( tr("Enable/Disable Polyline ROI tool") );
        toolAction->setIcon( QIcon(":/images/polyline.png") );
    }
    else
    {
        DEBUG_LOG(toolName + "> Tool no registrada!");
    }
    return toolAction;
}

}
