/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolmanager.h"
#include "toolregistry.h"
#include "tooldata.h"
#include "toolproxy.h"
#include "tool.h"
#include "qviewer.h"

#include <QStringList>
#include <QSignalMapper>
#include <QAction>
#include <QActionGroup>

namespace udg {

ToolManager::ToolManager(QObject *parent)
 : QObject(parent)
{
    // TODO de moment ToolRegistry és una classe normal, però si es passa a singleton
    // el "new" s'haurà de canviar per un ::instance()
    m_toolRegistry = new ToolRegistry(this);
    m_toolsActionSignalMapper = new QSignalMapper(this);
    connect( m_toolsActionSignalMapper, SIGNAL( mapped(const QString &) ), SIGNAL( triggeredToolAction(const QString &) ) );
}

ToolManager::~ToolManager()
{
}

void ToolManager::setViewerTools( QViewer *viewer, const QStringList &toolsList )
{
    QPair<QViewer *,ToolConfiguration *> pair;
    pair.first = viewer;
    pair.second = NULL;
    foreach( QString toolName, toolsList )
    {
        m_toolViewerMap.insert( toolName, pair );
    }
}

void ToolManager::setViewerTool( QViewer *viewer, const QString &toolName, ToolConfiguration *configuration )
{
    QPair<QViewer *,ToolConfiguration *> pair;
    pair.first = viewer;
    pair.second = configuration;
    m_toolViewerMap.insert( toolName, pair );
}

void ToolManager::addExclusiveToolsGroup( const QString &groupName, const QStringList &tools )
{
    QActionGroup *actionGroup = 0;
    if( m_toolsGroups.find( groupName ) != m_toolsGroups.end() )
    {
        // si el grup ja existeix, no cal crear l'entrada, simplement modificar-la
        actionGroup = m_toolsGroups.take( groupName );
    }
    else
    {
        actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);
    }
    // afegim el grup al mapa
    m_toolsGroups.insert(groupName, actionGroup);
    // per cada tool obtindrem la seva acció i la farem entrar en el grup d'exclusivitat
    foreach( QString toolName, tools )
    {
        // obtenim l'acció corresponent a aquella tool
        QAction *toolAction = qobject_cast<QAction *>( m_toolsActionSignalMapper->mapping(toolName) );
        actionGroup->addAction( toolAction );
    }
}

void ToolManager::activateTool( const QString &toolName )
{
    // TODO caldria comprovar si la tool es troba en un grup exclusiu per "fer fora" les altres tools? o es farà automàticament?
    QList< QPair<QViewer *, ToolConfiguration *> > viewerConfigList = m_toolViewerMap.values( toolName );

    ToolData *data = 0;
    QPair<QViewer *, ToolConfiguration *> pair;
    foreach( pair, viewerConfigList )
    {
        // declarem aquestes variables per fer-ho més llegible
        QViewer *viewer = pair.first;
        ToolConfiguration *configuration = pair.second;

        // obtenim la tool i la posem a punt amb les dades i la configuració
        Tool *tool = m_toolRegistry->getTool( toolName, viewer );
        if( tool->hasSharedData() )
        {
            if( !data ) // si data és nul, vol dir que és la primera tool que creem
                data = tool->getToolData();
            else
                tool->setToolData( data ); // si ja les hem creat abans, li assignem les de la primera tool creada
        }
        if( configuration ) // si no tenim cap configuració guardada, no cal fer res, es queda amb la que té per defecte
            tool->setConfiguration( configuration );
        // afegim la tool al proxy
        viewer->getToolProxy()->addTool( tool );
    }
}

void ToolManager::deactivateTool( const QString &toolName )
{
    QList< QPair<QViewer *, ToolConfiguration *> > viewerConfigList = m_toolViewerMap.values( toolName );

    QPair<QViewer *, ToolConfiguration *> pair;
    foreach( pair, viewerConfigList )
    {
        // declarem aquesta variable per fer-ho més llegible
        QViewer *viewer = pair.first;
        // eliminem la tool del proxy
        viewer->getToolProxy()->removeTool( toolName );
    }
}

void ToolManager::triggeredToolAction( const QString &toolName )
{
    // obtenim l'acció que l'ha provocat
    QAction *toolAction = qobject_cast<QAction *>( m_toolsActionSignalMapper->mapping(toolName) );
    // si està checked és que s'ha d'activar, altrament desactivar
    if( toolAction->isChecked() )
        activateTool(toolName);
    else
        deactivateTool(toolName);
}

QAction *ToolManager::getToolAction( const QString &toolName )
{
    QAction *toolAction = m_toolRegistry->getToolAction( toolName );
    m_toolsActionSignalMapper->setMapping( toolAction, toolName );
    connect( toolAction , SIGNAL( toggled() ) , m_toolsActionSignalMapper , SLOT( map() ) );
    return toolAction;
}

}
