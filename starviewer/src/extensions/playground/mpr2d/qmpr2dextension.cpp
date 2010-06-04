/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr2dextension.h"
#include "volume.h"
#include "q2dviewer.h"
#include "logging.h"
#include "toolmanager.h"
#include "toolproxy.h"
#include "windowlevelpresetstooldata.h"
#include "mpr2dsettings.h"
#include "patientbrowsermenu.h"
#include "toolconfiguration.h"
#include "imageplaneprojectiontool.h"
// qt
#include <QSpinBox> // pel control m_axialSpinBox
#include <QSlider> // pel control m_axialSlider
#include <QSplitter>

namespace udg {

QMPR2DExtension::QMPR2DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    MPR2DSettings().init(); 
    init();
    initializeTools();
    createConnections();
    readSettings();
    // ajustaments de window level pel combo box
    m_windowLevelComboBox->setPresetsData( m_axial2DView->getWindowLevelData() );
    m_coronal2DView->setWindowLevelData( m_axial2DView->getWindowLevelData() );
    m_sagital2DView->setWindowLevelData( m_axial2DView->getWindowLevelData() );
    m_windowLevelComboBox->selectPreset( m_axial2DView->getWindowLevelData()->getCurrentPreset() );
}

QMPR2DExtension::~QMPR2DExtension()
{
    writeSettings();

    delete m_coronal2DView;

    delete m_sagital2DView;
}

void QMPR2DExtension::init()
{
    // configurem les annotacions que volem veure
    m_axial2DView->removeAnnotation( Q2DViewer::ScalarBarAnnotation );
    m_coronal2DView->removeAnnotation( Q2DViewer::PatientOrientationAnnotation | Q2DViewer::ScalarBarAnnotation | Q2DViewer::PatientInformationAnnotation );
    m_sagital2DView->removeAnnotation( Q2DViewer::PatientOrientationAnnotation | Q2DViewer::ScalarBarAnnotation | Q2DViewer::PatientInformationAnnotation );

    m_coronal2DView->disableContextMenu();
    m_sagital2DView->disableContextMenu();
}

void QMPR2DExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_toolManager->registerTool("TranslateTool");
    m_toolManager->registerTool("WindowLevelTool");
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->registerTool("ScreenShotTool") );
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool("DistanceTool") );
    m_polylineROIToolButton->setDefaultAction( m_toolManager->registerTool("PolylineROITool") );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool("EraserTool") );

    m_thickSlabWidget->link( m_coronal2DView );
    
    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->registerTool("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // definim els grups exclusius
    m_leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PolylineROITool" << "EraserTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", m_leftButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RighttButtonGroup", rightButtonExclusiveTools);


    // Image Projection Tool
    m_toolManager->registerTool("ImagePlaneProjectionTool");
    QAction *imagePlaneProjectionToolAction = new QAction(this);
    imagePlaneProjectionToolAction->setIcon( m_toolManager->getRegisteredToolAction("ImagePlaneProjectionTool")->icon() );
    imagePlaneProjectionToolAction->setCheckable(true);
    m_imagePlaneProjectionToolButton->setDefaultAction(imagePlaneProjectionToolAction);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_imagePlaneProjectionToolButton->defaultAction()->trigger();
    m_toolManager->getRegisteredToolAction("TranslateTool")->trigger();
    m_toolManager->getRegisteredToolAction("WindowLevelTool")->trigger();
    
    // Assignem al viewer m_axial2DView els paràmetres per configurar la tool ImagePlaneProjectionTool com a productor
    ToolConfiguration *imagePlaneProjectionToolViwerAxialConfig= new ToolConfiguration();
    imagePlaneProjectionToolViwerAxialConfig->addAttribute( "typeConfiguration", QString ( "PRODUCER" ) );
    imagePlaneProjectionToolViwerAxialConfig->addAttribute( "numProjectedLines", 2 );    
    imagePlaneProjectionToolViwerAxialConfig->addAttribute( "namesProjectedLines", QVariant( ( QStringList () << "HORIZONTAL_LINE" << "VERTICAL_LINE" ) ) );
    imagePlaneProjectionToolViwerAxialConfig->addAttribute( "initOrientationsProjectedLines", QVariant(  ( QStringList () << "HORIZONTAL" << "VERTICAL" ) ) );
    QList< QVariant > axialColorsList;
    imagePlaneProjectionToolViwerAxialConfig->addAttribute( "colorsProjectedLines", QVariant(  ( axialColorsList << Qt::red << Qt::blue ) ) );
    m_toolManager->setViewerTool( m_axial2DView, "ImagePlaneProjectionTool", imagePlaneProjectionToolViwerAxialConfig );

    // Assignem al viewer m_coronal2DView els paràmetres per configurar la tool ImagePlaneProjectionTool com a consumidor
    ToolConfiguration *imagePlaneProjectionToolViewerCoronalConfig= new ToolConfiguration();
    imagePlaneProjectionToolViewerCoronalConfig->addAttribute( "typeConfiguration", QString ( "CONSUMER" ) );
    imagePlaneProjectionToolViewerCoronalConfig->addAttribute( "nameProjectedLine", QString ( "HORIZONTAL_LINE" ) );
    m_toolManager->setViewerTool( m_coronal2DView, "ImagePlaneProjectionTool", imagePlaneProjectionToolViewerCoronalConfig );

    // Assignem al viewer m_sagital2DView els paràmetres per configurar la tool ImagePlaneProjectionTool com a productor & consumidor
    ToolConfiguration *imagePlaneProjectionToolViewerSagitalConfig= new ToolConfiguration();
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "typeConfiguration", QString ( "PRODUCER&CONSUMER" ) );
    // Productor
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "numProjectedLines", QVariant( 1 ) );    
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "namesProjectedLines", QVariant( ( QStringList () << "HORIZONTAL_LINE" ) ) );
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "initOrientationsProjectedLines", QVariant(  ( QStringList () << "HORIZONTAL" ) ) );
    QList< QVariant > sagitalColorsList;
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "colorsProjectedLines", QVariant(  ( sagitalColorsList << Qt::red ) ) );
    // Consumidor
    imagePlaneProjectionToolViewerSagitalConfig->addAttribute( "nameProjectedLine", QString ( "VERTICAL_LINE" ) );
    m_toolManager->setViewerTool( m_sagital2DView, "ImagePlaneProjectionTool", imagePlaneProjectionToolViewerSagitalConfig );

    m_toolManager->getRegisteredToolAction("ImagePlaneProjectionTool")->trigger();   

    // registrem al manager les tools que van als diferents viewers
    initializeDefaultTools();

    //
    // Connexions "especials" per l'ImagePlaneProjectionTool
    //
    // TODO Tot això és un HACK per poder fer servir les eines alhora amb l'ImagePlaneProjectionTool
    //
    foreach (QString toolName, m_leftButtonExclusiveTools)
    {
        connect(m_toolManager->getRegisteredToolAction(toolName), SIGNAL(toggled(bool)), SLOT(onLeftButtonToolToggled(bool)) );
    }
    connect(imagePlaneProjectionToolAction, SIGNAL(triggered()), SLOT(disableLeftButtonTools()) );
    connect(imagePlaneProjectionToolAction, SIGNAL(toggled(bool)), SLOT(setEnabledImagePlaneProjectionTool(bool)) );
}

void QMPR2DExtension::initializeDefaultTools()
{
    QStringList toolsList1, toolsList2;
    toolsList1 << "ImagePlaneProjectionTool";
    toolsList2 << "ImagePlaneProjectionTool" << "ZoomTool" << "SlicingTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool" << "WindowLevelPresetsTool" << "DistanceTool" << "PolylineROITool" << "EraserTool";
    m_toolManager->setViewerTools( m_axial2DView, toolsList2 );
    m_toolManager->setViewerTools( m_coronal2DView, toolsList1 );
    m_toolManager->setViewerTools( m_sagital2DView, toolsList1 );
}

void QMPR2DExtension::createConnections()
{
    // conectem els sliders i demés visors
    // aquests tres connects es podrien resumir en un private slot : on_m_axialXXXX_valueChanged( int ) i aprofitaríem les característiques de l'auto connection
    connect( m_axialSlider , SIGNAL( valueChanged(int) ) , m_axialSpinBox , SLOT( setValue(int) ) );
    connect( m_axialSpinBox , SIGNAL( valueChanged(int) ) , m_axial2DView , SLOT( setSlice(int) ) );
    connect( m_axial2DView , SIGNAL( sliceChanged(int) ) , m_axialSlider , SLOT( setValue(int) ) );

    // layouts
    m_horizontalLayoutToolButton->setIcon( QIcon(":images/view_left_right.png") );
    connect( m_horizontalLayoutToolButton , SIGNAL( clicked() ), SLOT( switchHorizontalLayout() ) );

    // Fem que no s'assigni automàticament l'input que s'ha seleccionat amb el menú de pacient, ja que fem tractaments adicionals
    // sobre el volum seleccionat i l'input final del visor pot diferir de l'inicial i és l'extensió qui decideix finalment quin input
    // se li vol donar a cada viewer. Capturem la senyal de quin volum s'ha escollit i a partir d'aquí fem el que calgui
    disconnect( m_axial2DView->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_axial2DView, SLOT( setInput(Volume *) ) );
    connect( m_axial2DView->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), SLOT( setInput(Volume *) ) );
}

void QMPR2DExtension::switchHorizontalLayout()
{
    QWidget *leftWidget, *rightWidget;
    leftWidget = m_horizontalSplitter->widget( 0 );
    rightWidget = m_horizontalSplitter->widget( 1 );

    m_horizontalSplitter->insertWidget( 0 , rightWidget );
    m_horizontalSplitter->insertWidget( 1 , leftWidget );
}

void QMPR2DExtension::setInput( Volume *input )
{
    if( input->getNumberOfPhases() > 1 )
        m_phasesAlertLabel->setVisible(true);
    else
        m_phasesAlertLabel->setVisible(false);
     
    m_axial2DView->setInput( input );

    int extent[6];
    input->getWholeExtent( extent );
    // refrescar el controls
    m_axialSpinBox->setMinimum( extent[4] );
    m_axialSpinBox->setMaximum( extent[5] );
    m_axialSlider->setMaximum(  extent[5] );    
}

void QMPR2DExtension::readSettings()
{
    Settings settings;
    QString keyPrefix = "Starviewer-App-MPR/";

    if( settings.getValue( MPR2DSettings::HorizontalSplitterGeometry).toByteArray().isEmpty() )
    {
        QList<int> list;
        list << this->size().width()/2 << this->size().width()/2;
        m_horizontalSplitter->setSizes( list );
    }
    else
        settings.restoreGeometry( MPR2DSettings::HorizontalSplitterGeometry, m_horizontalSplitter );

    if( settings.getValue( MPR2DSettings::VerticalSplitterGeometry).toByteArray().isEmpty() )
    {
        QList<int> list;
        list << this->size().height()/2 << this->size().height()/2;
        m_verticalSplitter->setSizes( list );
    }
    else
        settings.restoreGeometry( MPR2DSettings::VerticalSplitterGeometry, m_verticalSplitter ); 

}

void QMPR2DExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry( MPR2DSettings::HorizontalSplitterGeometry, m_horizontalSplitter );
    settings.saveGeometry( MPR2DSettings::VerticalSplitterGeometry, m_verticalSplitter );
}

void QMPR2DExtension::disableLeftButtonTools()
{
    foreach (QString toolName, m_leftButtonExclusiveTools)
    {
        if ( m_toolManager->getRegisteredToolAction(toolName)->isChecked() )
        {
            m_toolManager->getRegisteredToolAction(toolName)->setChecked(false);
            m_toolManager->deactivateTool(toolName);
        }
    }
}

void QMPR2DExtension::setEnabledImagePlaneProjectionTool(bool enable)
{
    ( qobject_cast<ImagePlaneProjectionTool *>( m_axial2DView->getToolProxy()->getTool( "ImagePlaneProjectionTool" ) ) )->setEnabled( enable );
    ( qobject_cast<ImagePlaneProjectionTool *>( m_coronal2DView->getToolProxy()->getTool( "ImagePlaneProjectionTool" ) ) )->setEnabled( enable );
    ( qobject_cast<ImagePlaneProjectionTool *>( m_sagital2DView->getToolProxy()->getTool( "ImagePlaneProjectionTool" ) ) )->setEnabled( enable );
}

void QMPR2DExtension::onLeftButtonToolToggled(bool toggled)
{
    if ( toggled )
        m_imagePlaneProjectionToolButton->defaultAction()->setChecked(false);
}

};  //  end namespace udg
