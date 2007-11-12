/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dmprviewertoolmanager.h"
#include "zoomtool.h"
#include "translatetool.h"
#include "screenshottool.h"
#include "rotate3dtool.h"
#include "logging.h"

namespace udg {

Q3DMPRViewerToolManager::Q3DMPRViewerToolManager(QObject *parent)
 : OldToolManager(parent)
{
    m_viewer = 0;
    initToolRegistration();
}

Q3DMPRViewerToolManager::Q3DMPRViewerToolManager( Q3DMPRViewer *viewer , QObject *parent )
 : OldToolManager(parent)
{
    m_viewer = viewer;
    initToolRegistration();
}

Q3DMPRViewerToolManager::~Q3DMPRViewerToolManager()
{
    m_toolList.clear();
}

void Q3DMPRViewerToolManager::setViewer( Q3DMPRViewer *viewer )
{
    m_viewer = viewer;
    // \TODO caldria refrescar els viewers de cada tool!?
}

void Q3DMPRViewerToolManager::initToolRegistration()
{
    if( !m_viewer )
        DEBUG_LOG("ERROR:Inicialitzant tools amb un viewer NUL");

    m_toolList.clear();
    m_toolList["ZoomTool"] = new ZoomTool( m_viewer );
    m_toolList["TranslateTool"] = new TranslateTool( m_viewer );
    m_toolList["ScreenShotTool"] = new ScreenShotTool( m_viewer );
    m_toolList["3DRotationTool"] = new Rotate3DTool( m_viewer );
}

}
