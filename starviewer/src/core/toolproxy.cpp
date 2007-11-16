/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolproxy.h"
#include "tool.h"
#include <QString>
#include <QList>

namespace udg {

ToolProxy::ToolProxy(QObject *parent)
 : QObject(parent)
{
}

ToolProxy::~ToolProxy()
{
}

void ToolProxy::addTool( Tool *tool )
{
    m_toolsMap.insert( tool->toolName(), tool );
}

bool ToolProxy::removeTool( QString toolName )
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
    m_toolsMap.clear();
}

void ToolProxy::forwardEvent( unsigned long eventID )
{
    //no es pot fer un foreach sobre un map perquè retorna parella d'elements, per això passem tots els elements del map a una QList.
    QList <Tool*> toolsList = m_toolsMap.values();

    foreach( Tool *tool, toolsList )
    {
        tool->handleEvent( eventID );
    }
}

}
