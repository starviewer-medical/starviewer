/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dtiextension.h"

#include <QString>
#include "dtiextension.h"
#include "toolmanager.h"
#include "volume.h"

namespace udg {

DTIExtension::DTIExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_viewersLayout->addViewer( "0.0\\1.0\\1.0\\0.0" );
    m_viewersLayout->setSelectedViewer( m_viewersLayout->getViewerWidget(0) );

    createConnections();
    initializeTools();
}

DTIExtension::~DTIExtension()
{
    delete m_toolManager;
}

void DTIExtension::initializeTools()
{
    // Tools
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->registerTool("ScreenShotTool") );
    m_polylineButton->setDefaultAction( m_toolManager->registerTool( "PolylineROITool" ) );
    m_cursor3DToolButton->setDefaultAction( m_toolManager->registerTool("Cursor3DTool") );
    // registrem les eines de valors predefinits de window level, slicing per teclat i sincronització
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "ScreenShotTool" << "SeedTool" << "Cursor3DTool" << "EditorTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SlicingTool" << "WindowLevelTool" << "TranslateTool";
    m_toolManager->triggerTools(defaultTools);

    // TODO de moment fem exclusiu la tool de sincronització i la de cursor 3d manualment perque la
    // sincronització no té el model de totes les tools
    connect( m_toolManager->getRegisteredToolAction("Cursor3DTool"), SIGNAL( triggered() ), SLOT( disableSynchronization() ) );

    // Configurem els visors amb les tools configurades
    m_toolManager->setupRegisteredTools( m_viewersLayout->getViewerWidget(0)->getViewer() );
}

void DTIExtension::createConnections()
{

}

void DTIExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_viewersLayout->getViewerWidget(0)->setInput(input);
}

void DTIExtension::readDTIData( QString fileName )
{

}

}