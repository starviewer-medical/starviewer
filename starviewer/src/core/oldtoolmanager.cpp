/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "oldtoolmanager.h"
#include "qviewer.h"
#include "oldtool.h"
#include "logging.h"
namespace udg {

OldToolManager::OldToolManager(QObject *parent)
 : QObject(parent)
{
}

OldToolManager::~OldToolManager()
{
}

bool OldToolManager::setCurrentTool( QString toolName )
{
    if( m_toolList.find( toolName ) != m_toolList.end() )
    {
        m_currentToolName = toolName;
        return true;
    }
    else
        return false;
}

OldTool *OldToolManager::getTool( QString toolName )
{
    if( m_toolList.find( toolName ) != m_toolList.end() )
        return m_toolList[toolName];
    else
        return 0;
}

void OldToolManager::forwardEvent( unsigned long eventID )
{
    if( m_toolList.find( m_currentToolName ) != m_toolList.end() )
    {
        m_toolList[m_currentToolName]->handleEvent( eventID );
    }
}

}
