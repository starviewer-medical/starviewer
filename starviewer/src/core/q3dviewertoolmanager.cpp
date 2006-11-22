/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dviewertoolmanager.h"
#include "tool.h"
#include "zoomtool.h"
#include "translatetool.h"
#include "screenshottool.h"
#include "logging.h"

namespace udg {

Q3DViewerToolManager::Q3DViewerToolManager(QObject *parent)
 : ToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q3DViewerToolManager::Q3DViewerToolManager( Q3DViewer *viewer , QObject *parent )
 : ToolManager(parent)
{
    m_viewer = viewer;
    initToolRegistration();
}

Q3DViewerToolManager::~Q3DViewerToolManager()
{
    m_toolList.clear();
}

void Q3DViewerToolManager::setViewer( Q3DViewer *viewer )
{
    m_viewer = viewer;
    // \TODO caldria refrescar els viewers de cada tool!?
}

void Q3DViewerToolManager::initToolRegistration()
{
    if( !m_viewer )
        DEBUG_LOG("ERROR:Inicialitzant tools amb un viewer NUL");

    m_toolList.clear();
    m_toolList["ZoomTool"] = new ZoomTool( m_viewer );
    m_toolList["TranslateTool"] = new TranslateTool( m_viewer );
    m_toolList["ScreenShotTool"] = new ScreenShotTool( m_viewer );
}

}

