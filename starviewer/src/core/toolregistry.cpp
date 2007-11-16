/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolregistry.h"
#include "tool.h"
#include "zoomtool.h"
#include "qviewer.h"
#include "logging.h"

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
//     else if( toolName == "SlicingTool" )
//     {
//         tool = new SlicingTool( viewer );
//     }
//         ...
//         etc
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
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Window Level") );
        toolAction->setStatusTip( tr("Enable/Disable Window Level tool") );
        toolAction->setIcon( QIcon(":/images/windowLevel.png") );
    }
    else if( toolName == "ZoomTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Zoom") );
        toolAction->setStatusTip( tr("Enable/Disable Zoom tool") );
        toolAction->setIcon( QIcon(":/images/zoom.png") );
    }
    else if( toolName == "TranslateTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Move") );
        toolAction->setStatusTip( tr("Enable/Disable Move tool") );
        toolAction->setIcon( QIcon(":/images/move.png") );
    }
    else if( toolName == "SeedTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Seed") );
        toolAction->setStatusTip( tr("Put Seed tool") );
    }
    else if( toolName == "ScreenShotTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Screen Shot") );
        toolAction->setStatusTip( tr("Enable/Disable Screen shot tool") );
        toolAction->setIcon( QIcon(":/images/photo.png") );

    }
    else if( toolName == "3DRotationTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("3D Rotation") );
        toolAction->setStatusTip( tr("Enable/Disable 3D Rotation tool") );
        toolAction->setIcon( QIcon(":/images/rotate3d.png") );

    }
    else if( toolName == "DistanceTool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("Distances") );
        toolAction->setStatusTip( tr("Enable/Disable Distances tool") );
        toolAction->setIcon( QIcon(":/images/distance.png") );

    }
    else if( toolName == "ROITool" )
    {
        toolAction = new QAction( 0 );
        toolAction->setText( tr("ROI's") );
        toolAction->setStatusTip( tr("Enable/Disable ROI tool") );
        toolAction->setIcon( QIcon(":/images/roi.png") );

    }
    else
    {
        DEBUG_LOG(toolName + "> Tool no registrada!");
    }
    return toolAction;
}

}
