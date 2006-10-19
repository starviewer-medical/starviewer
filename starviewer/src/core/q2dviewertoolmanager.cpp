/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewertoolmanager.h"
#include "tool.h"
#include "slicing2dtool.h"
#include "windowleveltool.h"
#include "zoomtool.h"
#include "translatetool.h"

namespace udg {

Q2DViewerToolManager::Q2DViewerToolManager(QObject *parent)
 : ToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q2DViewerToolManager::Q2DViewerToolManager( Q2DViewer *viewer , QObject *parent )
 : ToolManager(parent)
{
    m_viewer = viewer;
    initToolRegistration();
}

Q2DViewerToolManager::~Q2DViewerToolManager()
{
    m_availableTools.clear();
}

void Q2DViewerToolManager::setViewer( Q2DViewer *viewer )
{
    m_viewer = viewer;
}

void Q2DViewerToolManager::forwardEvent( unsigned long eventID )
{
    if( m_currentTool )
        m_currentTool->handleEvent( eventID );
}

bool Q2DViewerToolManager::setCurrentTool( QString toolName )
{
    // no resetejem la tool si estem indiquem la mateixa
    if( m_currentTool )
    {    
        if( toolName != m_currentTool->getToolName() )
        {
            m_currentTool = this->createTool( toolName );
        }
        else
        {
            return false;
        }
    }
    else
    {
        m_currentTool = this->createTool( toolName );
    }
    return true;
}

Tool *Q2DViewerToolManager::createTool( QString toolName )
{
    ToolMapType::iterator current = m_availableTools.find( toolName );
    // si existeix llavors
    if( current != m_availableTools.end() )
    {
        switch( m_availableTools[toolName] )
        {
            // \TODO canviar els números per enums decents que ho identifiquen millor
            case 0:
                return new Slicing2DTool( m_viewer );
            break;
            
            case 1:
                return new WindowLevelTool( m_viewer );
            break;

            case 2:
                return new ZoomTool( m_viewer );
            break;

            case 3:
                return new TranslateTool( m_viewer );
            break;

            default:
            break;
        }
    }
    else
        return 0;
}

void Q2DViewerToolManager::initToolRegistration()
{
    // \TODO canviar els números per enums decents que ho identifiquen millor
    m_availableTools.clear();
    m_availableTools["SlicingTool"] = 0;
    m_availableTools["WindowLevelTool"] = 1;
    m_availableTools["ZoomTool"] = 2;
    m_availableTools["TranslateTool"] = 3;
}

}
