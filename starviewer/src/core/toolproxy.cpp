/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolproxy.h"
#include "tool.h"
#include "tooldata.h"
#include <QStringList>

namespace udg {

ToolProxy::ToolProxy(QObject *parent)
 : QObject(parent)
{
}

ToolProxy::~ToolProxy()
{
    removeAllTools();
    m_persistentToolDataRepository.clear();
}

void ToolProxy::addTool( Tool *tool )
{
    if( !m_toolsMap.contains(tool->toolName()) )
        m_toolsMap.insert( tool->toolName(), tool );
    else
    {
        delete tool;
    }
}

bool ToolProxy::removeTool( const QString &toolName )
{
    bool ok = false;
    if( m_toolsMap.contains( toolName ) )
    {
        // l'eliminem del mapa i alliberem la seva memòria
        Tool *tool = m_toolsMap.take( toolName );
        delete tool;
        ok = true;
    }
    return ok;
}

void ToolProxy::removeAllTools()
{
    QStringList toolsList = m_toolsMap.keys();
    foreach( QString toolName, toolsList )
    {
        Tool *tool = m_toolsMap.take( toolName );
        delete tool;
    }
}

bool ToolProxy::isToolActive( const QString &toolName )
{
    return m_toolsMap.contains( toolName );
}

Tool *ToolProxy::getTool( const QString &toolName ) 
{
    Tool *tool = 0;
    if( m_toolsMap.contains(toolName) )
        tool = m_toolsMap.value( toolName );

    if( tool )
    {
        // Si la tool demanada existeix, comprovem si té dades persistents
        if( tool->hasPersistentData() )
        {
            // mirem si les tenim al repositori
            ToolData *persistentData = m_persistentToolDataRepository.value( toolName );
            if( persistentData )
            {
                // hi són, per tant li assignem a la tool
                tool->setToolData( persistentData );
            }
            else
            {
                // no hi són al respositori, per tant és el primer cop que demanen la tool
                // obtenim les seves dades i les registrem al repositori
                m_persistentToolDataRepository[toolName] = tool->getToolData();
            }
        }
    }
    return tool;
}

void ToolProxy::forwardEvent( unsigned long eventID )
{
    //no es pot fer un foreach sobre un map perquè retorna parella d'elements, per això passem tots els elements del map a una QList.
    QList<Tool *> toolsList = m_toolsMap.values();

    foreach( Tool *tool, toolsList )
    {
        tool->handleEvent( eventID );
    }
}

}
