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
#include "screenshottool.h"
#include "distancetool.h"
#include "roitool.h"
#include "logging.h"
#include "seedtool.h"

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
    m_toolList.clear();
}

void Q2DViewerToolManager::setViewer( Q2DViewer *viewer )
{
    m_viewer = viewer;
    // \TODO caldria refrescar els viewers de cada tool!?
}

void Q2DViewerToolManager::initToolRegistration()
{
    if( !m_viewer )
        DEBUG_LOG("ERROR:Inicialitzant tools amb un viewer NUL");

    m_toolList.clear();
    m_toolList["SlicingTool"] = new Slicing2DTool( m_viewer );
    m_toolList["WindowLevelTool"] = new WindowLevelTool( m_viewer );
    m_toolList["ZoomTool"] = new ZoomTool( m_viewer );
    m_toolList["TranslateTool"] = new TranslateTool( m_viewer );
    m_toolList["ScreenShotTool"] = new ScreenShotTool( m_viewer );
    m_toolList["SeedTool"] = new SeedTool( m_viewer );
    m_toolList["DistanceTool"] = new DistanceTool( m_viewer );
    m_toolList["ROITool"] = new ROITool( m_viewer );
}

}
