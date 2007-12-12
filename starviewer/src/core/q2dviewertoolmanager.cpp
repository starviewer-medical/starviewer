/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewertoolmanager.h"
#include "slicing2dtool.h"
#include "oldwindowleveltool.h"
#include "oldzoomtool.h"
#include "oldtranslatetool.h"
#include "oldscreenshottool.h"
#include "olddistancetool.h"
#include "roitool.h"
#include "logging.h"
#include "oldseedtool.h"

#include "q2dviewer.h"

namespace udg {

Q2DViewerToolManager::Q2DViewerToolManager(QObject *parent)
 : OldToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q2DViewerToolManager::Q2DViewerToolManager( Q2DViewer *viewer , QObject *parent )
 : OldToolManager(parent)
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
    m_toolList["WindowLevelTool"] = new OldWindowLevelTool( m_viewer );
    m_toolList["ZoomTool"] = new OldZoomTool( m_viewer );
    m_toolList["TranslateTool"] = new OldTranslateTool( m_viewer );
    m_toolList["ScreenShotTool"] = new OldScreenShotTool( m_viewer );
    m_toolList["SeedTool"] = new OldSeedTool( m_viewer );
    m_toolList["DistanceTool"] = new OldDistanceTool( m_viewer );
    m_toolList["ROITool"] = new ROITool( m_viewer );
}

}
