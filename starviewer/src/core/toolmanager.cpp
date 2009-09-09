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
#include "toolconfiguration.h" // pel delete
#include "tool.h"
#include "qviewer.h"
#include "logging.h"

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
    connect( m_toolsActionSignalMapper, SIGNAL( mapped(const QString &) ), SLOT( triggeredToolAction(const QString &) ) );
}

ToolManager::~ToolManager()
{
}

void ToolManager::setViewerTools( QViewer *viewer, const QStringList &toolsList )
{
    ViewerToolConfigurationPairType pair;
    pair.first = viewer;
    pair.second = NULL;
    foreach( QString toolName, toolsList )
    {
        m_toolViewerMap.insert( toolName, pair );
    }
    refreshConnections(); //TODO Xapussilla, s'hauria de fer amb tractament més bo intern, amb llistes de tools actives
}

void ToolManager::setupRegisteredTools( QViewer *viewer )
{
    setViewerTools(viewer,getRegisteredToolsList());
}

void ToolManager::setViewerTool( QViewer *viewer, const QString &toolName, ToolConfiguration *configuration )
{
    ViewerToolConfigurationPairType pair;
    pair.first = viewer;
    pair.second = configuration;
    m_toolViewerMap.insert( toolName, pair );
    refreshConnections(); //TODO Xapussilla, s'hauria de fer amb tractament més bo intern, amb llistes de tools actives
}

void ToolManager::removeViewerTool( QViewer *viewer, const QString &toolName )
{
    QMutableMapIterator<QString, ViewerToolConfigurationPairType> mapIterator( m_toolViewerMap );
    bool found = false;
    while( mapIterator.hasNext() && !found )
    {
        mapIterator.next();
        if( mapIterator.key() == toolName )
        {
            ViewerToolConfigurationPairType pair = mapIterator.value();
            if( pair.first == viewer )
            {
                found = true;
                // eliminem la tool del proxy
                viewer->getToolProxy()->removeTool( toolName );
                // això vol dir que per un nom de tool, nomé spodem tenir un parell viewer-config
                // del contrari, ens hauríem de "patejar" tot el map sencer per si tenim la mateixa tool,
                // pel mateix viewer i amb una configuració diferent
                if( pair.second ) // eliminem la configuració
                    delete pair.second;

                mapIterator.remove(); // eliminem l'element del mapa
            }
        }
    }
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
        QAction *toolAction = getRegisteredToolAction(toolName);
        if( toolAction )
            actionGroup->addAction( toolAction );
        else
            DEBUG_LOG( QString("No tenim registrada cap Action per la tool ") + toolName );
    }
    // guarrada! TODO aixo es un workaround per poder desactivar "automaticament" les tools dins d'un mateix grup
    // lo correcte seria plantejar be el tema dels signals mappers o fer una implementacio propia mes elaborada
    connect( actionGroup, SIGNAL(triggered(QAction *)), SLOT(refreshConnections()) );
}

QAction *ToolManager::registerActionTool( const QString &actionToolName )
{
    QPair<QAction *, QString> pair;
    // si no està registrada la obtenim del registre i l'afegim al nostre map
    if( !m_actionToolRegistry.contains( actionToolName ) )
    {
        pair = m_toolRegistry->getActionToolPair( actionToolName );
        m_actionToolRegistry.insert( actionToolName, pair );
    }
    else // sinó, l'agafem del nostre map i no tornem a crear l'acció
    {
        pair = m_actionToolRegistry.value( actionToolName );
    }

    return pair.first;
}

void ToolManager::triggerTools( const QStringList &toolsList )
{
    foreach(QString toolName, toolsList )
    {
        triggerTool( toolName );
    }
}

void ToolManager::triggerTool( const QString &toolName )
{
    if( m_toolsActionsRegistry.contains( toolName ) )
    {
        m_toolsActionsRegistry.value(toolName)->trigger();
    }
}

void ToolManager::enableActionTools( QViewer *viewer, const QStringList &actionToolsList )
{
    QPair<QAction *, QString> pair;
    foreach( QString actionToolName, actionToolsList )
    {
        pair = m_actionToolRegistry.value(actionToolName);
        connect( pair.first, SIGNAL( triggered() ), viewer, qPrintable(pair.second) );
    }
}

void ToolManager::disableActionTools( QViewer *viewer, const QStringList &actionToolsList )
{
    QPair<QAction *, QString> pair;
    foreach( QString actionToolName, actionToolsList )
    {
        pair = m_actionToolRegistry.value(actionToolName);
        disconnect( pair.first, SIGNAL( triggered() ), viewer, 0 );
    }
}

void ToolManager::enableRegisteredActionTools( QViewer *viewer )
{
    enableActionTools( viewer, m_actionToolRegistry.keys() );
}

void ToolManager::disableRegisteredActionTools( QViewer *viewer )
{
    disableActionTools( viewer, m_actionToolRegistry.keys() );
}

void ToolManager::disableAllToolsTemporarily()
{
    QStringList toolsList = m_toolViewerMap.uniqueKeys();
    foreach(QString toolName, toolsList)
    {
        deactivateTool(toolName);
    }
}

void ToolManager::undoDisableAllToolsTemporarily()
{
    refreshConnections();
}

void ToolManager::activateTool( const QString &toolName )
{
    // TODO caldria comprovar si la tool es troba en un grup exclusiu per "fer fora" les altres tools 
    // en el cas que prescindíssim del mecanisme que fem servir amb QActionToolGroup
    QList< ViewerToolConfigurationPairType > viewerConfigList = m_toolViewerMap.values( toolName );

    ToolData *data = m_sharedToolDataRepository.value( toolName );
    // declarem aquestes variables per fer-ho més llegible
    QViewer *viewer;
    ToolConfiguration *configuration;
    foreach( ViewerToolConfigurationPairType pair, viewerConfigList )
    {
        viewer = pair.first;
        configuration = pair.second;
        Tool *tool = 0;

        // hem de comprovar si el proxy ja té o no la tool,
        if( !viewer->getToolProxy()->isToolActive( toolName ) )
        {
            // com que el proxy no té aquesta tool
            // la produim i la posem a punt amb les dades i la configuració
            tool = m_toolRegistry->getTool( toolName, viewer );
            if( configuration ) // si no tenim cap configuració guardada, no cal fer res, es queda amb la que té per defecte
                tool->setConfiguration( configuration );
            // afegim la tool al proxy
            viewer->getToolProxy()->addTool( tool );
            // comprovem les dades per si cal donar-n'hi
            if( tool->hasSharedData() )
            {
                if( !data ) // no hi són al repositori, les obtindrem de la pròpia tool i les registrarem al repositori
                {
                    data = tool->getToolData();
                    m_sharedToolDataRepository[ toolName ] = data;
                }
                else
                    tool->setToolData( data ); // si ja les hem creat abans, li assignem les de la primera tool creada
            }
        }
    }
}

void ToolManager::deactivateTool( const QString &toolName )
{
    QList< ViewerToolConfigurationPairType > viewerConfigList = m_toolViewerMap.values( toolName );

    foreach( ViewerToolConfigurationPairType pair, viewerConfigList )
    {
        // declarem aquesta variable per fer-ho més llegible
        QViewer *viewer = pair.first;
        // eliminem la tool del proxy
        viewer->getToolProxy()->removeTool( toolName );
    }
    // elinimen Shared Data d'aquesta tool
    m_sharedToolDataRepository.remove( toolName );
}

void ToolManager::triggeredToolAction( const QString &toolName )
{
    // TODO: Cal repassar tot això. Hauria d'anar amb llistes internes de tools activades/desactivades
    // obtenim l'acció que l'ha provocat
    QAction *toolAction = getRegisteredToolAction(toolName);
    if( toolAction )
    {
        // si està checked és que s'ha d'activar, altrament desactivar
        if( toolAction->isChecked() )
            activateTool(toolName);
        else
            deactivateTool(toolName);
    }
    else
    {
        DEBUG_LOG( QString("No hi ha cap tool Action per la tool anomenada: ") + toolName );
    }
}

QAction *ToolManager::getRegisteredToolAction( const QString &toolName )
{
    QAction *toolAction = 0;
    if( m_toolsActionsRegistry.contains(toolName) )
    {
        // si ja existeix l'obtenim del registre de tools/acció
        toolAction = m_toolsActionsRegistry.value(toolName);
    }
    return toolAction;
}

QAction *ToolManager::registerTool( const QString &toolName )
{
    QAction *toolAction;
    if( m_toolsActionsRegistry.contains(toolName) )
    {
        // si ja existeix l'obtenim del registre de tools/acció
        toolAction = m_toolsActionsRegistry.value(toolName);
    }
    else
    {
        // altrament, creem l'acció associada, la connectem amb l'estructura interna i la registrem
        toolAction = m_toolRegistry->getToolAction( toolName );
        m_toolsActionSignalMapper->setMapping( toolAction, toolName );
        connect( toolAction, SIGNAL( triggered() ), m_toolsActionSignalMapper, SLOT( map() ) );
        m_toolsActionsRegistry.insert( toolName, toolAction );
    }
    // retornem l'acció associada
    return toolAction;
}

QStringList ToolManager::getRegisteredToolsList() const
{
    return m_toolsActionsRegistry.uniqueKeys();
}

void ToolManager::refreshConnections()
{
    QStringList toolsList = getRegisteredToolsList();
    foreach( QString tool, toolsList )
    {
        triggeredToolAction( tool );
    }
}

}
