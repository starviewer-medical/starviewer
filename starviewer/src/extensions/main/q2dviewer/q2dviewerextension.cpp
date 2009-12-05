/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextension.h"

#include "volume.h"
#include "image.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "q2dviewerwidget.h"
#include "menugridwidget.h"
#include "tablemenu.h"
#include "qpreviousstudieswidget.h"
#include "patient.h"
#include "study.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "windowlevelpresetstooldata.h"
#include "qdicomdump.h"
#include "hangingprotocolmanager.h"
#include "statswatcher.h"
// per poder fer screenshots desde menú
#include "screenshottool.h" 
#include "toolproxy.h"
#include "q2dviewersettings.h"
#include "qexportertool.h"

#include <QMenu>
#include <QWidgetAction>
#include <QAction>
#include <QPoint>
#include <QGridLayout>
#include <QProgressDialog>
#include <QMessageBox>

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_patient(0), m_lastSelectedViewer(0) 
{
    setupUi( this );
    Q2DViewerSettings().init();

#ifdef STARVIEWER_LITE
    
    m_polylineButton->hide();
    m_angleToolButton->hide();
    m_axialViewToolButton->hide();
    m_coronalViewToolButton->hide();
    m_sagitalViewToolButton->hide();
    label_2->hide();
    label_4->hide();
    m_thickSlabWidget->hide();
    m_referenceLinesToolButton->hide();
    m_cursor3DToolButton->hide();
    m_flipHorizontalToolButton->setVisible(true);
    m_flipVerticalToolButton->setVisible(true);

#else

    m_flipVerticalToolButton->setVisible( false );
    m_flipHorizontalToolButton->setVisible( false );

#endif

    //TODO ocultem botons que no son del tot necessaris o que no es faran servir
    m_windowLevelToolButton->setVisible(false);
    m_translateToolButton->setVisible(false);

    m_predefinedSeriesGrid = new MenuGridWidget(this);
    m_seriesTableGrid = new TableMenu(this);
    m_predefinedSlicesGrid = new MenuGridWidget(this);
    m_sliceTableGrid = new TableMenu(this);
    m_dicomDumpCurrentDisplayedImage = new QDicomDump(this);

    readSettings();
    createConnections();

    m_previousStudiesToolButton->setEnabled(false);
    // TODO de moment no fem accessible aquesta funcionalitat ja que no està a punt
    m_imageGrid->setVisible(false);
    m_downImageGrid->setVisible(false);
    initializeTools();

    m_flipVerticalToolButton->setVisible(true);
    m_flipHorizontalToolButton->setVisible(true);

    // incorporem estadístiques
    m_statsWatcher = new StatsWatcher("2D Extension",this);
    m_statsWatcher->addClicksCounter( m_slicingToolButton );
    m_statsWatcher->addClicksCounter( m_zoomToolButton );
    m_statsWatcher->addClicksCounter( m_polylineButton );
    m_statsWatcher->addClicksCounter( m_distanceToolButton );
    m_statsWatcher->addClicksCounter( m_angleToolButton );
    m_statsWatcher->addClicksCounter( m_eraserToolButton );

    m_statsWatcher->addClicksCounter( m_axialViewToolButton );
    m_statsWatcher->addClicksCounter( m_sagitalViewToolButton );
    m_statsWatcher->addClicksCounter( m_coronalViewToolButton );

    m_statsWatcher->addClicksCounter( m_dicomDumpToolButton );
    m_statsWatcher->addClicksCounter( m_viewerInformationToolButton );

    m_statsWatcher->addClicksCounter( m_rotateClockWiseToolButton );
    m_statsWatcher->addClicksCounter( m_flipHorizontalToolButton );
    m_statsWatcher->addClicksCounter( m_flipVerticalToolButton );
    
    m_statsWatcher->addClicksCounter( m_cursor3DToolButton );
    m_statsWatcher->addClicksCounter( m_referenceLinesToolButton );

    m_statsWatcher->addClicksCounter( m_previousStudiesToolButton );
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();

    m_hangingCandidates.clear();
    delete m_predefinedSeriesGrid;
    delete m_seriesTableGrid;
    delete m_predefinedSlicesGrid;
    delete m_sliceTableGrid;
    delete m_dicomDumpCurrentDisplayedImage;
    // L'objecte es crea quan fem un setInput. Per tant, fem la comprovació.
    if ( !m_previousStudiesWidget )
        delete m_previousStudiesWidget;
}

void Q2DViewerExtension::createConnections()
{
    // Menus
    connect( m_downButtonGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedGrid() ) );
    connect( m_buttonGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveTable() ) );
    connect( m_downImageGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedImageGrid() ) );
    connect( m_imageGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveImageTable() ) );

    // Connexions del menu
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int , int ) ), m_workingArea , SLOT( setGrid( int, int ) ) );
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int ) ), this, SLOT( setHangingProtocol( int ) ) );
    connect( m_seriesTableGrid, SIGNAL( selectedGrid( int , int ) ), m_workingArea, SLOT( setGrid( int, int ) ) );

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_viewerInformationToolButton, SIGNAL( toggled( bool ) ), SLOT( showViewerInformation( bool ) ) );
    // per mostrar la informació DICOM de la imatge que s'està veient en aquell moment
    connect( m_dicomDumpToolButton, SIGNAL( clicked() ) , SLOT( showDicomDumpCurrentDisplayedImage() ) );

    // per mostrar exportació
    connect( m_screenshotsExporterToolButton, SIGNAL( clicked() ) , SLOT( showScreenshotsExporterDialog() ) );

    // Connexions necessaries amb els canvis al layout
    connect( m_workingArea, SIGNAL( viewerAdded( Q2DViewerWidget * ) ), SLOT( activateNewViewer( Q2DViewerWidget * ) ) );
    connect( m_workingArea, SIGNAL( viewerSelectedChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    /// Aplicació dels hanging protocols
    HangingProtocolManager * hangingProtocolManger = new HangingProtocolManager();

    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_hangingCandidates = hangingProtocolManger->searchAndApplyBestHangingProtocol( m_workingArea, m_patient );
	delete hangingProtocolManger;
	hangingProtocolManger = 0;
    QApplication::restoreOverrideCursor();

    if( m_hangingCandidates.size() == 0 ) // No hi ha hanging protocols
    {
        Q2DViewerWidget *viewerWidget = m_workingArea->addViewer( "0.0\\1.0\\1.0\\0.0" );
        viewerWidget->setInput( m_mainVolume );
    }
    m_workingArea->setViewerSelected( m_workingArea->getViewerWidget(0) );
    m_predefinedSeriesGrid->setHangingItems( m_hangingCandidates );

    connect( m_patient, SIGNAL( patientFused() ), SLOT(searchHangingProtocols()) );

    /// Habilitem la possibilitat de buscar estudis previs.
    m_previousStudiesWidget = new QPreviousStudiesWidget( m_mainVolume->getStudy() , this );

    QMenu *previousStudiesMenu = new QMenu;
    QWidgetAction *previousStudiesWidgetAction = new QWidgetAction(this);
    previousStudiesWidgetAction->setDefaultWidget(m_previousStudiesWidget);
    previousStudiesMenu->addAction(previousStudiesWidgetAction);

    if ( m_previousStudiesToolButton->menu() )
        delete m_previousStudiesToolButton->menu();
    m_previousStudiesToolButton->setMenu( previousStudiesMenu );

    m_previousStudiesToolButton->setEnabled( true );
}

void Q2DViewerExtension::searchHangingProtocols()
{
    HangingProtocolManager * hangingProtocolManger = new HangingProtocolManager();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_hangingCandidates = hangingProtocolManger->searchAndApplyBestHangingProtocol( m_workingArea, m_patient );
	delete hangingProtocolManger;
	hangingProtocolManger = 0;
    QApplication::restoreOverrideCursor();
    m_predefinedSeriesGrid->setHangingItems( m_hangingCandidates );
}

void Q2DViewerExtension::showPredefinedGrid()
{
    int i;
    int numberSeries = 0;

    QPoint point = m_buttonGrid->mapToGlobal( QPoint(0,0) );
    m_predefinedSeriesGrid->move( point.x(),( point.y() + m_buttonGrid->frameGeometry().height() ) );

    QList<Study *> listStudies = m_patient->getStudies();

    for( i = 0; i < listStudies.size(); i++ )
    {
        numberSeries += listStudies.value( i )->getNumberOfSeries();
    }

    m_predefinedSeriesGrid->createPredefinedGrids( numberSeries );
    m_predefinedSeriesGrid->show();

}

void Q2DViewerExtension::showInteractiveTable()
{
    QPoint point = m_buttonGrid->mapToGlobal( QPoint(0,0) );
    m_seriesTableGrid->move( point.x(),( point.y() + m_buttonGrid->frameGeometry().height() ) );
    m_seriesTableGrid->show();
}

void Q2DViewerExtension::showPredefinedImageGrid()
{
    QPoint point = m_imageGrid->mapToGlobal( QPoint(0,0) );
    m_predefinedSlicesGrid->move( point.x(),( point.y() + m_imageGrid->frameGeometry().height() ) );
    m_predefinedSlicesGrid->createPredefinedGrids( m_workingArea->getViewerSelected()->getViewer()->getMaximumSlice() );
    m_predefinedSlicesGrid->show();
}

void Q2DViewerExtension::showInteractiveImageTable()
{
    QPoint point = m_imageGrid->mapToGlobal( QPoint(0,0) );
    m_sliceTableGrid->move( point.x(),( point.y() + m_imageGrid->frameGeometry().height() ) );
    m_sliceTableGrid->show();
}

Patient* Q2DViewerExtension::getPatient() const
{
    return m_patient;
}

void Q2DViewerExtension::setPatient( Patient *patient )
{
    m_patient = patient;
    // Ara és super txapussa i només mirarà  el primer estudi
    Study *study = m_patient->getStudies().first();
    if( study )
    {
        if( study->getModalities().contains("MG") || study->getModalities().contains("CR") || study->getModalities().contains("RF") || study->getModalities().contains("OP") )
        {
            m_slicingToolButton->defaultAction()->toggle();
            m_zoomToolButton->defaultAction()->trigger();
        }
        else
        {
            m_slicingToolButton->defaultAction()->trigger();
        }
    }
}

void Q2DViewerExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_referenceLinesToolButton->setDefaultAction( m_toolManager->registerTool("ReferenceLinesTool") );
    m_polylineButton->setDefaultAction( m_toolManager->registerTool( "PolylineROITool" ) );
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool( "DistanceTool" ) );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool( "EraserTool" ) );
    m_cursor3DToolButton->setDefaultAction( m_toolManager->registerTool("Cursor3DTool") );
    m_angleToolButton->setDefaultAction( m_toolManager->registerTool( "AngleTool" ) );
    m_openAngleToolButton->setDefaultAction( m_toolManager->registerTool( "NonClosedAngleTool" ) );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool( "VoxelInformationTool" ) );
    // registrem les eines de valors predefinits de window level, slicing per teclat i sincronització
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("SynchronizeTool");

    // registrem les "Action Tool"    
    m_sagitalViewAction = m_toolManager->registerActionTool( "SagitalViewActionTool" );
    m_coronalViewAction = m_toolManager->registerActionTool( "CoronalViewActionTool" );
    m_axialViewToolButton->setDefaultAction( m_toolManager->registerActionTool( "AxialViewActionTool" ) );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateClockWiseActionTool" ) );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateCounterClockWiseActionTool" ) );
    m_flipHorizontalToolButton->setDefaultAction( m_toolManager->registerActionTool( "HorizontalFlipActionTool" ) );
    m_flipVerticalToolButton->setDefaultAction( m_toolManager->registerActionTool( "VerticalFlipActionTool" ) );
    m_restoreToolButton->setDefaultAction( m_toolManager->registerActionTool( "RestoreActionTool" ) );
    m_invertToolButton->setDefaultAction( m_toolManager->registerActionTool( "InvertWindowLevelActionTool" ) );

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "DistanceTool" << "EraserTool" << "AngleTool" << "NonClosedAngleTool" << "Cursor3DTool";
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

    //
    // Casos especials de Tools
    //

    // TODO de moment fem exclusiu la tool de sincronització i la de cursor 3d manualment perque la
    // sincronització no té el model de totes les tools
    connect( m_toolManager->getRegisteredToolAction("Cursor3DTool"), SIGNAL( triggered() ), SLOT( disableSynchronization() ) );

    // SCREEN SHOT TOOL
    // activem l'eina d'screen shot, que sempre estarà activa
    // TODO tot això es podria convertir més endavant en dues Action Tools en comptes d'aquesta Tool
    m_screenShotTriggerAction = m_toolManager->registerTool("ScreenShotTool");
    m_toolManager->triggerTool("ScreenShotTool");
    // fem que l'screen shot sigui una mica més acessible afegint-li un menú en el que podem escollir quina acció realitzar
    // d'alguna manera tot això són una mica uns HACKS ja que el mecanisme de funcionament d'aquesta tool és una mica diferent
    // i caldria tenir en compte tools d'aquests tipus per donar-li cabuda en la infraestructura de tools.
    m_screenShotToolButton->setPopupMode( QToolButton::InstantPopup );
    m_screenShotToolButton->setCheckable(false);
    m_singleShotAction = new QAction( this );
    m_singleShotAction->setText( tr("Save current series image") );
    m_singleShotAction->setShortcut( Qt::CTRL + Qt::Key_S );
    m_singleShotAction->setToolTip( tr("Save the current image in a standard image format") );
    
    m_multipleShotAction = new QAction( this );
    m_multipleShotAction->setText( tr("Save all images from current series") );
    m_multipleShotAction->setToolTip( tr("Save all the images in the selected viewer in a standard image format") );
    m_multipleShotAction->setShortcut( Qt::CTRL + Qt::Key_A );    
    
    QMenu *screenShotMenu = new QMenu(this);
    m_screenShotToolButton->setMenu( screenShotMenu );
    
    screenShotMenu->addAction( m_singleShotAction );
    screenShotMenu->addAction( m_multipleShotAction );

    m_screenShotToolButton->setIcon( m_screenShotTriggerAction->icon() );
    m_screenShotToolButton->setToolTip( m_screenShotTriggerAction->toolTip() );
    m_screenShotToolButton->setText( m_screenShotTriggerAction->text() );
}

void Q2DViewerExtension::activateNewViewer( Q2DViewerWidget * newViewerWidget)
{
     // i si cal, activem les annotacions
    if( m_viewerInformationToolButton->isChecked() )
        newViewerWidget->getViewer()->enableAnnotation( Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation |
        Q2DViewer::RulersAnnotation | Q2DViewer::SliceAnnotation | Q2DViewer::PatientInformationAnnotation |
        Q2DViewer::AcquisitionInformationAnnotation, true );

    connect( newViewerWidget, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );

    // li indiquem les tools que li hem configurat per defecte a tothom
    m_toolManager->setupRegisteredTools( newViewerWidget->getViewer() );
}

void Q2DViewerExtension::changeSelectedViewer( Q2DViewerWidget *viewerWidget )
{
    if( !viewerWidget )
    {
        DEBUG_LOG("El Viewer donat és NUL!");
        return;
    }
    if ( viewerWidget != m_lastSelectedViewer )
    {
        ///TODO canviar aquestes connexions i desconnexions per dos mètodes el qual
        /// enviin el senyal al visualitzador que toca.
        if( m_lastSelectedViewer )
        {
            disconnect( m_lastSelectedViewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), this, SLOT( validePhases() ) );
            disconnect( m_lastSelectedViewer->getViewer(), SIGNAL( viewChanged(int) ), this, SLOT( updateDICOMInformationButton(int) ) );
            // és necessari associar cada cop al viewer actual les associacions del menú de la tool d'screen shot
            ScreenShotTool *screenShotTool = dynamic_cast<ScreenShotTool *>( m_lastSelectedViewer->getViewer()->getToolProxy()->getTool("ScreenShotTool") );
            disconnect( m_singleShotAction, SIGNAL( triggered() ), screenShotTool, SLOT( singleCapture() ) );
            disconnect( m_multipleShotAction, SIGNAL( triggered() ), screenShotTool, SLOT( completeCapture() ) );
            // desactivem les "ActionTool" pel visor que acaba de deseleccionar-se
            m_toolManager->disableRegisteredActionTools( m_lastSelectedViewer->getViewer() );
        }
        m_lastSelectedViewer = viewerWidget;
        Q2DViewer *selected2DViewer = viewerWidget->getViewer();
        
        validePhases();
        connect( viewerWidget->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );
        connect( viewerWidget->getViewer(), SIGNAL( viewChanged(int) ), SLOT( updateDICOMInformationButton(int) ) );
        // és necessari associar cada cop al viewer actual les associacions del menú de la tool d'screen shot
        ScreenShotTool *screenShotTool = dynamic_cast<ScreenShotTool *>( viewerWidget->getViewer()->getToolProxy()->getTool("ScreenShotTool") );
        if( screenShotTool )
        {
            connect( m_singleShotAction, SIGNAL( triggered() ), screenShotTool, SLOT( singleCapture() ) );
            connect( m_multipleShotAction, SIGNAL( triggered() ), screenShotTool, SLOT( completeCapture() ) );
        }

        // TODO potser hi hauria alguna manera més elegant, com tenir un slot a WindowLevelPresetsToolData
        // que es digués activateCurrentPreset() i el poguéssim connectar a algun signal
        m_windowLevelComboBox->setPresetsData( selected2DViewer->getWindowLevelData() );
        m_windowLevelComboBox->selectPreset( selected2DViewer->getWindowLevelData()->getCurrentPreset() );

        m_cineController->setQViewer( selected2DViewer );
        m_thickSlabWidget->link( selected2DViewer );
        updateDICOMInformationButton( selected2DViewer->getView() );

        // activem les "ActionTool" pel visor seleccionat
        m_toolManager->enableRegisteredActionTools( selected2DViewer );
    }
}

void Q2DViewerExtension::showViewerInformation( bool show )
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();
    int numViewer;

    for( numViewer = 0; numViewer < numberOfViewers; numViewer++ )
    {
       m_workingArea->getViewerWidget( numViewer )->getViewer()->enableAnnotation( Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation |
        Q2DViewer::RulersAnnotation | Q2DViewer::SliceAnnotation | Q2DViewer::PatientInformationAnnotation |
        Q2DViewer::AcquisitionInformationAnnotation, show );
    }
}

void Q2DViewerExtension::showDicomDumpCurrentDisplayedImage()
{
    m_dicomDumpCurrentDisplayedImage->setCurrentDisplayedImage( m_workingArea->getViewerSelected()->getViewer()->getCurrentDisplayedImage() );
    m_dicomDumpCurrentDisplayedImage->show();
}

void Q2DViewerExtension::showScreenshotsExporterDialog()
{
    if ( m_workingArea->getViewerSelected()->getViewer()->getInput() == NULL )
    {
        QMessageBox::warning(this, tr("Export to DICOM") , tr("This action is not allowed because the selected viewer is empty.") );
        return;
    }

    QExporterTool exporter( m_workingArea->getViewerSelected()->getViewer() );
    exporter.exec();
}

void Q2DViewerExtension::validePhases()
{
    if( m_workingArea->getViewerSelected()->hasPhases() )
    {
        m_sagitalViewAction->setEnabled( false );
        m_coronalViewAction->setEnabled( false );
    }
    else
    {
        m_sagitalViewAction->setEnabled( true );
        m_coronalViewAction->setEnabled( true );
    }
}

void Q2DViewerExtension::updateDICOMInformationButton( int view )
{
    if( m_workingArea->getViewerSelected()->getViewer()->getInput() )
    {
        if( view == Q2DViewer::Axial )
            m_dicomDumpToolButton->setEnabled(true);
        else
            m_dicomDumpToolButton->setEnabled(false);
    }
    else
    {
        m_dicomDumpToolButton->setEnabled(false);
    }
}

void Q2DViewerExtension::readSettings()
{
    Settings settings;
    m_profile = settings.getValue( Q2DViewerSettings::UserProfile ).toString();
}

void Q2DViewerExtension::writeSettings()
{
}

void Q2DViewerExtension::synchronization( Q2DViewerWidget * viewer, bool active )
{
    if( active )
    {
        // Per defecte sincronitzem només la tool de slicing
        ToolConfiguration *synchronizeConfiguration = new ToolConfiguration();
        synchronizeConfiguration->addAttribute( "Slicing", QVariant( true ) );
        m_toolManager->setViewerTool( viewer->getViewer(), "SynchronizeTool", synchronizeConfiguration );
        m_toolManager->activateTool("SynchronizeTool");

        // TODO si el cursor 3d està seleccionat, el deseleccionem. 
        // Solució temporal, hauríem d'incorporar algun mecanisme a ToolManager per gestionar aquests casos
        if( m_cursor3DToolButton->isChecked () ) // TODO en comptes de comprovar si la tool està activada via "botó" es podria incorporar algun mecanisme a ToolManager que ens digués si una tool està activada o no
            m_toolManager->triggerTool("SlicingTool");
    }
    else
    {
        m_toolManager->removeViewerTool( viewer->getViewer(), "SynchronizeTool" );
    }
}

void Q2DViewerExtension::disableSynchronization()
{
    // TODO Mètode per desactivar l'eina i el boto de sincronització dels visualitzadors quan
    // es selecciona l'eina de cursor3D, per solucionar-ho de forma xapussa perquè l'eina de
    // sincronització encara no té el mateix format que la resta
    int numViewer;
    Q2DViewerWidget * viewer;

    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for( numViewer = 0; numViewer < numberOfViewers; numViewer++ )
    {
        viewer =  m_workingArea->getViewerWidget( numViewer );
        m_toolManager->removeViewerTool( viewer->getViewer(), "SynchronizeTool" );
        viewer->disableSynchronization();
    }
}

void Q2DViewerExtension::setHangingProtocol( int hangingProtocolNumber )
{
    /// Aplicació dels hanging protocols
    HangingProtocolManager * hangingProtocolManger = new HangingProtocolManager();
    hangingProtocolManger->applyHangingProtocol( hangingProtocolNumber, m_workingArea );
}

}
