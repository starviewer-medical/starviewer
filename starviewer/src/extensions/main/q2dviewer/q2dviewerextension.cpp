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
#include "patient.h"
#include "study.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "windowlevelpresetstooldata.h"
#include "qdicomdump.h"
#include "statswatcher.h"
// per poder fer screenshots desde menú
#include "screenshottool.h" 
#include "toolproxy.h"
#include "q2dviewersettings.h"

#ifndef STARVIEWER_LITE
#include "qpreviousstudieswidget.h"
#include "hangingprotocolmanager.h"
#include "previousstudiesmanager.h"
#include "qexportertool.h"
#endif

#include <QMenu>
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
    m_openAngleToolButton->hide();
    m_axialViewToolButton->hide();
    m_coronalViewToolButton->hide();
    m_sagitalViewToolButton->hide();
    m_orientationButtonsLabel->hide();
    m_thickSlabLabel->hide();
    m_thickSlabWidget->hide();
    m_referenceLinesToolButton->hide();
    m_cursor3DToolButton->hide();
    m_previousStudiesToolButton->hide();
    m_screenshotsExporterToolButton->hide();
#else
    m_hangingProtocolManager = 0;
#endif

    //TODO ocultem botons que no son del tot necessaris o que no es faran servir
    m_windowLevelToolButton->setVisible(false);
    m_translateToolButton->setVisible(false);

    m_predefinedSeriesGrid = new MenuGridWidget(this);
    m_seriesTableGrid = new TableMenu(this);
    m_predefinedSlicesGrid = new MenuGridWidget(this);
    m_sliceTableGrid = new TableMenu(this);
    m_dicomDumpCurrentDisplayedImage = new QDicomDump(this);
    
#ifndef STARVIEWER_LITE
    m_previousStudiesWidget = new QPreviousStudiesWidget( this );
    m_previousStudiesToolButton->setEnabled(false);
    m_previousStudiesToolButton->setToolTip( tr("Search prior studies") );
    m_screenshotsExporterToolButton->setToolTip( tr("Export viewer image(s) to DICOM and send them to a PACS server") );
#endif
    
    m_viewerInformationToolButton->setToolTip( tr("Show/Hide viewer's textual information") );
    m_dicomDumpToolButton->setToolTip( tr("Dump DICOM information of the current image") );
    m_windowLevelComboBox->setToolTip( tr("Choose Window/Level Presets") );
    
    // TODO de moment no fem accessible aquesta funcionalitat ja que no està a punt
    m_imageGrid->setVisible(false);
    m_downImageGrid->setVisible(false);

    readSettings();
    createConnections();
    initializeTools();

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

#ifndef STARVIEWER_LITE
    m_hangingCandidates.clear();
    // L'objecte es crea quan fem un setInput. Per tant, fem la comprovació.
    if ( !m_previousStudiesWidget )
        delete m_previousStudiesWidget;

    if( !m_hangingProtocolManager )
        delete m_hangingProtocolManager;
#endif

    delete m_predefinedSeriesGrid;
    delete m_seriesTableGrid;
    delete m_predefinedSlicesGrid;
    delete m_sliceTableGrid;
    delete m_dicomDumpCurrentDisplayedImage;
}

void Q2DViewerExtension::createConnections()
{
    // Menus
    connect( m_downButtonGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedGrid() ) );
    connect( m_buttonGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveTable() ) );
    connect( m_downImageGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedImageGrid() ) );
    connect( m_imageGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveImageTable() ) );

    // Connexions del menu
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int , int ) ), this , SLOT( setGrid( int, int ) ) );
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int ) ), this, SLOT( setHangingProtocol( int ) ) );
    connect( m_seriesTableGrid, SIGNAL( selectedGrid( int , int ) ), this, SLOT( setGrid( int, int ) ) );

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_viewerInformationToolButton, SIGNAL( toggled( bool ) ), SLOT( showViewerInformation( bool ) ) );
    // per mostrar la informació DICOM de la imatge que s'està veient en aquell moment
    connect( m_dicomDumpToolButton, SIGNAL( clicked() ) , SLOT( showDicomDumpCurrentDisplayedImage() ) );

    // Connexions necessaries amb els canvis al layout
    connect( m_workingArea, SIGNAL( viewerAdded( Q2DViewerWidget * ) ), SLOT( activateNewViewer( Q2DViewerWidget * ) ) );
    connect( m_workingArea, SIGNAL( viewerSelectedChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );

#ifndef STARVIEWER_LITE
    // per mostrar exportació
    connect( m_screenshotsExporterToolButton, SIGNAL( clicked() ) , SLOT( showScreenshotsExporterDialog() ) );
    
    connect( m_previousStudiesWidget, SIGNAL( downloadingStudies() ), this, SLOT( changeToPreviousStudiesDownloadingIcon() ) );
    connect( m_previousStudiesWidget, SIGNAL( studiesDownloaded() ), this, SLOT( changeToPreviousStudiesDefaultIcon() ) );
    connect( m_previousStudiesToolButton, SIGNAL( clicked ( bool ) ), SLOT( showPreviousStudiesWidget() ) );
#endif

}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;

#ifdef STARVIEWER_LITE
    Q2DViewerWidget *viewerWidget = m_workingArea->addViewer( "0.0\\1.0\\1.0\\0.0" );
    viewerWidget->setInput( m_mainVolume );
#else
    /// Aplicació dels hanging protocols
    if( m_hangingProtocolManager != 0 )
    {
        m_hangingProtocolManager->cancelHangingProtocolDowloading();
        delete m_hangingProtocolManager;
    }
    m_hangingProtocolManager = new HangingProtocolManager();

    connect( m_patient, SIGNAL( patientFused() ), SLOT(searchHangingProtocols()) );

    QApplication::setOverrideCursor( Qt::WaitCursor );
    int bestHangingProtocolIndex;
    m_hangingCandidates = m_hangingProtocolManager->searchHangingProtocols( m_workingArea, m_patient, false, &bestHangingProtocolIndex );
    
    if ( bestHangingProtocolIndex > -1 )
    {
        m_hangingProtocolManager->applyHangingProtocol( m_hangingCandidates[bestHangingProtocolIndex], m_workingArea, m_patient );
    }
    QApplication::restoreOverrideCursor();

    if( m_hangingCandidates.size() == 0 ) // No hi ha hanging protocols
    {
        Q2DViewerWidget *viewerWidget = m_workingArea->addViewer( "0.0\\1.0\\1.0\\0.0" );
        viewerWidget->setInput( m_mainVolume );
    }
    m_workingArea->setViewerSelected( m_workingArea->getViewerWidget(0) );
    m_predefinedSeriesGrid->setHangingItems( m_hangingCandidates );

    /// Habilitem la possibilitat de buscar estudis previs.
    m_previousStudiesToolButton->setEnabled( true );
    searchPreviousStudiesOfMostRecentStudy();

    searchPreviousStudiesWithHangingProtocols();
#endif
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::searchAndApplyBestHangingProtocol()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_hangingCandidates = m_hangingProtocolManager->searchHangingProtocols( m_workingArea, m_patient, true );
    QApplication::restoreOverrideCursor();
}

void Q2DViewerExtension::searchHangingProtocols()
{
    m_hangingCandidates = m_hangingProtocolManager->searchHangingProtocols( m_workingArea, m_patient, false );
    m_predefinedSeriesGrid->setHangingItems( m_hangingCandidates );
    searchPreviousStudiesWithHangingProtocols();
}

void Q2DViewerExtension::searchPreviousStudiesWithHangingProtocols()
{
    m_predefinedSeriesGrid->setSearchingItem( true );

    //2.- Creacio de la classe per trobar previes
    m_previousStudiesManager = new PreviousStudiesManager();

    //3.- Es connecta el signal per quan acabi
    connect( m_previousStudiesManager, SIGNAL( queryPreviousStudiesFinished(QList<Study*>, QHash<QString, QString>) ), SLOT(addPreviousHangingProtocols(QList<Study*>, QHash<QString, QString>) ) );

    //4.- Es busquen els previs
    m_previousStudiesManager->queryPreviousStudies( m_mainVolume->getStudy() );
}

void Q2DViewerExtension::addPreviousHangingProtocols( QList<Study*> studies, QHash<QString, QString> qhash )
{
    disconnect( m_previousStudiesManager, SIGNAL( queryPreviousStudiesFinished(QList<Study*>, QHash<QString, QString>) ), this, SLOT(addPreviousHangingProtocols(QList<Study*>, QHash<QString, QString>) ) );

    QList<HangingProtocol * > m_hangingWidthPrevious = m_hangingProtocolManager->getHangingProtocolsWidthPreviousSeries( m_patient, studies, qhash );
    m_hangingCandidates << m_hangingWidthPrevious;
    m_predefinedSeriesGrid->addHangingItems( m_hangingWidthPrevious );
    m_predefinedSeriesGrid->createHangingProtocolsWidget();
    m_predefinedSeriesGrid->setSearchingItem( false );
}
#endif

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
    m_predefinedSeriesGrid->createHangingProtocolsWidget();
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

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::showPreviousStudiesWidget()
{
    QPoint point = m_previousStudiesToolButton->mapToGlobal( QPoint(0,0) );
    m_previousStudiesWidget->move( point.x(),( point.y() + m_previousStudiesToolButton->frameGeometry().height() ) );
    m_previousStudiesWidget->show();
}
#endif

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
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool( "DistanceTool" ) );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool( "EraserTool" ) );
#ifndef STARVIEWER_LITE
    m_polylineButton->setDefaultAction( m_toolManager->registerTool( "PolylineROITool" ) );
    m_cursor3DToolButton->setDefaultAction( m_toolManager->registerTool("Cursor3DTool") );
    m_angleToolButton->setDefaultAction( m_toolManager->registerTool( "AngleTool" ) );
    m_openAngleToolButton->setDefaultAction( m_toolManager->registerTool( "NonClosedAngleTool" ) );
#endif
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool( "VoxelInformationTool" ) );
    // registrem les eines de valors predefinits de window level, slicing per teclat i sincronització
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("SynchronizeTool");

    // registrem les "Action Tool"    
#ifndef STARVIEWER_LITE
    m_sagitalViewAction = m_toolManager->registerActionTool( "SagitalViewActionTool" );
    m_coronalViewAction = m_toolManager->registerActionTool( "CoronalViewActionTool" );
    m_axialViewToolButton->setDefaultAction( m_toolManager->registerActionTool( "AxialViewActionTool" ) );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );
#endif
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateClockWiseActionTool" ) );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateCounterClockWiseActionTool" ) );
    m_flipHorizontalToolButton->setDefaultAction( m_toolManager->registerActionTool( "HorizontalFlipActionTool" ) );
    m_flipVerticalToolButton->setDefaultAction( m_toolManager->registerActionTool( "VerticalFlipActionTool" ) );
    m_restoreToolButton->setDefaultAction( m_toolManager->registerActionTool( "RestoreActionTool" ) );
    m_invertToolButton->setDefaultAction( m_toolManager->registerActionTool( "InvertWindowLevelActionTool" ) );
    // Afegim un menú al botó d'erase per incorporar l'acció d'esborrar tot el que hi ha al visor
    m_eraserToolButton->setPopupMode( QToolButton::MenuButtonPopup );
    QMenu *eraserToolMenu = new QMenu(this);
    m_eraserToolButton->setMenu( eraserToolMenu );
    eraserToolMenu->addAction( m_toolManager->registerActionTool( "ClearCurrentViewAnnotationsActionTool" ) );

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;

#ifdef STARVIEWER_LITE
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "EraserTool";
#else
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "DistanceTool" << "EraserTool" << "AngleTool" << "NonClosedAngleTool" << "Cursor3DTool";
#endif
    
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
#ifndef STARVIEWER_LITE
    // TODO de moment fem exclusiu la tool de sincronització i la de cursor 3d manualment perque la
    // sincronització no té el model de totes les tools
    connect( m_toolManager->getRegisteredToolAction("Cursor3DTool"), SIGNAL( triggered() ), SLOT( disableSynchronization() ) );
#endif

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

#ifndef STARVIEWER_LITE
            disconnect( m_lastSelectedViewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), this, SLOT( validePhases() ) );
#endif

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

#ifndef STARVIEWER_LITE
        validePhases();
        connect( viewerWidget->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );
#endif
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

#ifndef STARVIEWER_LITE
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
#endif

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

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::setHangingProtocol( int hangingProtocolNumber )
{
    m_hangingProtocolManager->applyHangingProtocol( hangingProtocolNumber, m_workingArea, m_patient );
}

void Q2DViewerExtension::changeToPreviousStudiesDownloadingIcon()
{
    m_previousStudiesToolButton->setIcon( QIcon( QString(":images/cal_downloading.png") ) );
}

void Q2DViewerExtension::changeToPreviousStudiesDefaultIcon()
{
    m_previousStudiesToolButton->setIcon( QIcon( QString(":images/cal.png") ) );
}

void Q2DViewerExtension::searchPreviousStudiesOfMostRecentStudy()
{
    Study * recentStudy = NULL;

    if( m_mainVolume ) // Només busquem si hi ha el volum principal
    {
        foreach ( Study * study, m_mainVolume->getPatient()->getStudies() )
        {
            if ( recentStudy == NULL )
            {
                recentStudy = study;
            }
            else
            {
                if ( study->getDateTime() > recentStudy->getDateTime() )
                {
                    recentStudy = study;
                }
            }
        }
        m_previousStudiesWidget->searchPreviousStudiesOf( recentStudy );
    }
}
#endif

void Q2DViewerExtension::setGrid( int rows , int columns )
{
#ifndef STARVIEWER_LITE
    m_hangingProtocolManager->cancelHangingProtocolDowloading();
#endif
    m_workingArea->setGrid( rows , columns );
}

}
