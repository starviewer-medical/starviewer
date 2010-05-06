/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "curvedmprextension.h"

#include "curvedmprsettings.h"
#include "toolmanager.h"

namespace udg {

CurvedMPRExtension::CurvedMPRExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    CurvedMPRSettings().init();
    
    // TODO corretgir aquesta inicialització inicial, això no hauria de ser necessari
    m_viewersLayout->addViewer( "0.0\\1.0\\1.0\\0.0" );
    m_viewersLayout->setGrid(2,1);
    
    initializeTools();
}

CurvedMPRExtension::~CurvedMPRExtension()
{
}

void CurvedMPRExtension::initializeTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_scrollToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool("DistanceTool") );
    m_polylineROIToolButton->setDefaultAction( m_toolManager->registerTool("PolylineROITool") );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool("EraserTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    // Tools sense botó
    m_toolManager->registerTool("WindowLevelTool");
    m_toolManager->registerTool("TranslateTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("WindowLevelPresetsTool");
    // and so on...

    // Definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PolylineROITool" << "EraserTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingTool" << "WindowLevelTool" << "TranslateTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools(defaultTools);

    // Registrem al manager les tools que van als diferents viewers
    m_toolManager->setupRegisteredTools( m_viewersLayout->getViewerWidget(0)->getViewer() );
    m_toolManager->setupRegisteredTools( m_viewersLayout->getViewerWidget(1)->getViewer() );
}

void CurvedMPRExtension::setInput( Volume *input )
{
    m_viewersLayout->getViewerWidget(0)->getViewer()->setInput(input);
    m_viewersLayout->getViewerWidget(1)->getViewer()->setInput(input);
}

};  // end namespace udg


