/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolsactionfactory.h"
#include <QAction>
#include <QSignalMapper>

namespace udg {

ToolsActionFactory::ToolsActionFactory( QWidget *parent )
 : QObject( parent )
{
    m_signalMapper = new QSignalMapper( this );
    
    m_slicingAction = new QAction( 0 );
    m_slicingAction->setText( tr("Slicer") );
    m_slicingAction->setStatusTip( tr("Enable/Disable slicing tool") );
    m_slicingAction->setIcon( QIcon(":/images/slicing.png") );
    m_slicingAction->setCheckable( true );
    m_signalMapper->setMapping( m_slicingAction , "SlicingTool" );
    connect( m_slicingAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    m_windowLevelAction = new QAction( 0 );
    m_windowLevelAction->setText( tr("Window Level") );
    m_windowLevelAction->setStatusTip( tr("Enable/Disable Window Level tool") );
    m_windowLevelAction->setIcon( QIcon(":/images/windowLevel.png") );
    m_windowLevelAction->setCheckable( true );
    m_signalMapper->setMapping( m_windowLevelAction , "WindowLevelTool" );
    connect( m_windowLevelAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    m_zoomAction = new QAction( 0 );
    m_zoomAction->setText( tr("Zoom") );
    m_zoomAction->setStatusTip( tr("Enable/Disable Zoom tool") );
    m_zoomAction->setIcon( QIcon(":/images/zoom.png") );
    m_zoomAction->setCheckable( true );
    m_signalMapper->setMapping( m_zoomAction , "ZoomTool" );
    connect( m_zoomAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    m_moveAction = new QAction( 0 );
    m_moveAction->setText( tr("Move") );
    m_moveAction->setStatusTip( tr("Enable/Disable Move tool") );
    m_moveAction->setIcon( QIcon(":/images/move.png") );
    m_moveAction->setCheckable( true );
    m_signalMapper->setMapping( m_moveAction , "TranslateTool" );
    connect( m_moveAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    connect( m_signalMapper, SIGNAL( mapped(QString) ), this , SIGNAL( triggeredTool(QString) ) );
    
    // \TODO canviar els números per enums decents que ho identifiquen millor
    m_availableToolActions.clear();
    m_availableToolActions["SlicingTool"] = 0;
    m_availableToolActions["WindowLevelTool"] = 1;
    m_availableToolActions["ZoomTool"] = 2;
    m_availableToolActions["TranslateTool"] = 3;
}

ToolsActionFactory::~ToolsActionFactory()
{
}

QAction *ToolsActionFactory::getActionFrom( QString toolName )
{
    if( m_availableToolActions.find( toolName ) != m_availableToolActions.end() )
    {
        switch( m_availableToolActions[toolName] )
        {
            // \TODO canviar els números per enums decents que ho identifiquen millor
            case 0:
                return m_slicingAction;
            break;
            
            case 1:
                return m_windowLevelAction;
            break;

            case 2:
                return m_zoomAction;
            break;

            case 3:
                return m_moveAction;
            break;

            default:
            break;
        }
    }
    else
        return 0;
}



}
