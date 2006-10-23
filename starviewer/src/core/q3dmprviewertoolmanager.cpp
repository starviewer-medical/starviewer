/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dmprviewertoolmanager.h"
#include "tool.h"
#include "zoomtool.h"
#include "translatetool.h"

namespace udg {

Q3DMPRViewerToolManager::Q3DMPRViewerToolManager(QObject *parent)
 : ToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q3DMPRViewerToolManager::Q3DMPRViewerToolManager( Q3DMPRViewer *viewer , QObject *parent )
 : ToolManager(parent)
{
    m_viewer = viewer;
    initToolRegistration();
}

Q3DMPRViewerToolManager::~Q3DMPRViewerToolManager()
{
    m_availableTools.clear();
}

void Q3DMPRViewerToolManager::setViewer( Q3DMPRViewer *viewer )
{
    m_viewer = viewer;
}

void Q3DMPRViewerToolManager::forwardEvent( unsigned long eventID )
{
    if( m_currentTool )
        m_currentTool->handleEvent( eventID );
}

bool Q3DMPRViewerToolManager::setCurrentTool( QString toolName )
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

Tool *Q3DMPRViewerToolManager::createTool( QString toolName )
{
    ToolMapType::iterator current = m_availableTools.find( toolName );
    // si existeix llavors
    if( current != m_availableTools.end() )
    {
        switch( m_availableTools[toolName] )
        {
            // \TODO canviar els números per enums decents que ho identifiquen millor
            case 0:
                return new ZoomTool( m_viewer );
            break;

            case 1:
                return new TranslateTool( m_viewer );
            break;

            default:
            break;
        }
    }
    else
        return 0;
}

void Q3DMPRViewerToolManager::initToolRegistration()
{
    // \TODO canviar els números per enums decents que ho identifiquen millor
    m_availableTools.clear();
    m_availableTools["ZoomTool"] = 0;
    m_availableTools["TranslateTool"] = 1;
}

}
