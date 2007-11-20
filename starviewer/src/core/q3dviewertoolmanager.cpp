/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dviewertoolmanager.h"
#include "oldzoomtool.h"
#include "oldtranslatetool.h"
#include "oldscreenshottool.h"
#include "oldrotate3dtool.h"
#include "logging.h"
#include "q3dviewer.h"

namespace udg {

Q3DViewerToolManager::Q3DViewerToolManager(QObject *parent)
 : OldToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q3DViewerToolManager::Q3DViewerToolManager( Q3DViewer *viewer , QObject *parent )
 : OldToolManager(parent)
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
    m_toolList["ZoomTool"] = new OldZoomTool( m_viewer );
    m_toolList["TranslateTool"] = new OldTranslateTool( m_viewer );
    m_toolList["ScreenShotTool"] = new OldScreenShotTool( m_viewer );
    m_toolList["3DRotationTool"] = new OldRotate3DTool( m_viewer );
}

}

