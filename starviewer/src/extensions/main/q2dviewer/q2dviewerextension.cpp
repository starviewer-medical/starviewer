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
#include "hangingprotocolmanager.h"

#include <QAction>
#include <QSettings>
#include <QPoint>
#include <QGridLayout>
#include <QProgressDialog>
// EXTRA!!! \TODO es temporal
#include <QFileDialog>
#include "keyimagenote.h"
#include "q2dviewerkeyimagenoteattacher.h"
#include "q2dviewerpresentationstateattacher.h"

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
 : QWidget( parent ), m_presentationStateAttacher(0), m_viewer(0)
{
    setupUi( this );

    //TODO ocultem botons que no son del tot necessaris o que no es faran servir
    m_rotateCounterClockWiseToolButton->setVisible( false );
    m_flipVerticalToolButton->setVisible( false );
    m_flipHorizontalToolButton->setVisible( false );
    m_windowLevelToolButton->setVisible(false);
    m_translateToolButton->setVisible(false);

    // TODO deshabilitem els presentation states fins la release en què es tornin a habilitar
    m_presentationStateSwitchToolButton->setVisible(false);

    m_mainVolume = 0;
    m_keyImageNoteAttacher = NULL;
    m_keyImageNote = NULL;

    m_patient = NULL;

    m_predefinedSeriesGrid = new MenuGridWidget();
    m_seriesTableGrid = new TableMenu();
    m_predefinedSlicesGrid = new MenuGridWidget();
    m_sliceTableGrid = new TableMenu();
    m_dicomDumpCurrentDisplayedImage = new QDicomDump();

    createProgressDialog();
    readSettings();
    createActions();
    createConnections();

    // TODO de moment no fem accessible aquesta funcionalitat ja que no està a punt
    m_imageGrid->setVisible(false);
    m_downImageGrid->setVisible(false);
    initializeTools();

    activateNewViewer( m_workingArea->getViewerSelected() );
    changeSelectedViewer( m_workingArea->getViewerSelected() );

    //TODO "Xapussa" del ticket #599 per tal de crear el perfil per CR o MG
    if( m_profile == "ProfileOnlyCR")
    {
        m_slicingToolButton->defaultAction()->toggle();
        m_zoomToolButton->defaultAction()->trigger();
        m_flipVerticalToolButton->setVisible(true);
        m_flipHorizontalToolButton->setVisible(true);
        m_cineController->setVisible(false);
    }
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();

    delete m_predefinedSeriesGrid;
    delete m_seriesTableGrid;
    delete m_predefinedSlicesGrid;
    delete m_sliceTableGrid;
    delete m_dicomDumpCurrentDisplayedImage;

}

void Q2DViewerExtension::createActions()
{
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("Axial") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    m_axialViewToolButton->setDefaultAction( m_axialViewAction );

    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("Sagital") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Saggital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );

    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("Coronal") );
    m_coronalViewAction->setStatusTip( tr("Change Current View To Coronal") );
    m_coronalViewAction->setIcon( QIcon(":/images/coronal.png") );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );

    // per activar i desactivar els presentation states
    m_presentationStateAction = new QAction( 0 );
    m_presentationStateAction->setText( tr("PS") );
    m_presentationStateAction->setStatusTip( tr("Enable/Disable the current attached") );
    m_presentationStateAction->setCheckable( true );
    m_presentationStateAction->setEnabled(false);
    m_presentationStateAction->setChecked(false);
    m_presentationStateSwitchToolButton->setDefaultAction( m_presentationStateAction );

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction, SIGNAL( triggered() ), SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction, SIGNAL( triggered() ), SLOT( rotateCounterClockWise() ) );

    m_flipHorizontalAction = new QAction(0);
    m_flipHorizontalAction->setText( tr("Flip Horizontal") );
    m_flipHorizontalAction->setStatusTip( tr("Flip the image horizontally") );
    m_flipHorizontalAction->setIcon( QIcon(":/images/flipHorizontal.png") );
    m_flipHorizontalToolButton->setDefaultAction( m_flipHorizontalAction );

    connect( m_flipHorizontalAction , SIGNAL( triggered() ), SLOT( horizontalFlip() ) );

    m_flipVerticalAction = new QAction(0);
    m_flipVerticalAction->setText( tr("Flip Vertical") );
    m_flipVerticalAction->setStatusTip( tr("Flip the image vertically") );
    m_flipVerticalAction->setIcon( QIcon(":/images/flipVertical.png") );
    m_flipVerticalToolButton->setDefaultAction( m_flipVerticalAction );

    connect( m_flipVerticalAction , SIGNAL( triggered() ), SLOT( verticalFlip() ) );
}

void Q2DViewerExtension::enablePresentationState(bool enable)
{
    if( enable )
    {
        m_presentationStateAttacher->attach();
    }
    else
    {
        m_presentationStateAttacher->detach();
        this->setInput( m_mainVolume );
    }
}

void Q2DViewerExtension::createConnections()
{
    // adicionals, TODO ara es fa "a saco" però s'ha de millorar, ens podríem estalviar un slot i fer la connexió cada cop que
    // se selecciona un de nou directament amb els slots del Q2DViewer
    connect( m_axialViewAction, SIGNAL( triggered() ), SLOT( resetViewToAxial() ) );
    connect( m_sagitalViewAction, SIGNAL( triggered() ), SLOT( resetViewToSagital() ) );
    connect( m_coronalViewAction, SIGNAL( triggered() ), SLOT( resetViewToCoronal() ) );

    // Menus
    connect( m_downButtonGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedGrid() ) );
    connect( m_buttonGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveTable() ) );
    connect( m_downImageGrid, SIGNAL( clicked ( bool ) ), SLOT( showPredefinedImageGrid() ) );
    connect( m_imageGrid, SIGNAL( clicked ( bool ) ), SLOT( showInteractiveImageTable() ) );

    // Connexions del menu
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int , int ) ), m_workingArea , SLOT( setGrid( int, int ) ) );
    connect( m_seriesTableGrid, SIGNAL( selectedGrid( int , int ) ), m_workingArea, SLOT( setGrid( int, int ) ) );

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), SLOT( enablePresentationState(bool) ) );

    // Connexions necessaries pel primer visualitzador
    connect( m_workingArea->getViewerSelected()->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_viewerInformationToolButton, SIGNAL( toggled( bool ) ), SLOT( showViewerInformation( bool ) ) );

    connect( m_dicomDumpToolButton, SIGNAL( clicked() ) , SLOT( showDicomDumpCurrentDisplayedImage() ) );

    // Connexions necessaries amb els canvis al layout
    connect( m_workingArea, SIGNAL( viewerAdded( Q2DViewerWidget * ) ), SLOT( activateNewViewer( Q2DViewerWidget * ) ) );
    connect( m_workingArea, SIGNAL( viewerSelectedChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
	m_mainVolume = input;
	INFO_LOG("Q2DViewerExtension: Donem l'input principal");

    /// Aplicació dels hanging protocols
    HangingProtocolManager * hangingProtocolManger = new HangingProtocolManager();
	bool existHanging = hangingProtocolManger->searchAndApplyBestHangingProtocol( m_workingArea, m_patient );

	if( !existHanging )
	{
		m_workingArea->getViewerSelected()->setInput( m_mainVolume );
	}
}

void Q2DViewerExtension::resetViewToAxial()
{
    m_workingArea->getViewerSelected()->resetViewToAxial();
}

void Q2DViewerExtension::resetViewToSagital()
{
    m_workingArea->getViewerSelected()->resetViewToSagital();
}

void Q2DViewerExtension::resetViewToCoronal()
{
    m_workingArea->getViewerSelected()->resetViewToCoronal();
}

void Q2DViewerExtension::loadKeyImageNote(const QString &filename)
{
    if (m_keyImageNote != NULL)
    {
        delete m_keyImageNote;
    }
    m_keyImageNote = new KeyImageNote();
    if ( ! m_keyImageNote->loadFromFile(filename))
    {
        DEBUG_LOG( "ERROR! Al llegir el KIN " + filename );
        return;
    }

    // Es carrega l'attacher per el viewer principal
    if ( m_keyImageNoteAttacher != NULL)
    {
        delete m_keyImageNoteAttacher;
    }
    m_keyImageNoteAttacher = new Q2DViewerKeyImageNoteAttacher(  m_workingArea->getViewerSelected()->getViewer(), m_keyImageNote );
    m_keyImageNoteAttacher->setVisibleAdditionalInformation( true );
    m_keyImageNoteAttacher->attach();
}

void Q2DViewerExtension::loadPresentationState(const QString &filename)
{
    // Es carrega l'attacher per el viewer principal només
    if( m_presentationStateAttacher != NULL )
    {
        delete m_presentationStateAttacher;
    }
    m_presentationStateAttacher = new Q2DViewerPresentationStateAttacher(  m_workingArea->getViewerSelected()->getViewer(), qPrintable(filename) );
    m_presentationStateAction->setEnabled( true );
    m_presentationStateAction->setChecked( true );
}

void Q2DViewerExtension::rotateClockWise()
{
    ( m_workingArea->getViewerSelected()->getViewer() )->rotateClockWise();
}

void Q2DViewerExtension::rotateCounterClockWise()
{
    ( m_workingArea->getViewerSelected()->getViewer() )->rotateCounterClockWise();
}

void Q2DViewerExtension::horizontalFlip()
{
    ( m_workingArea->getViewerSelected()->getViewer() )->horizontalFlip();
}

void Q2DViewerExtension::verticalFlip()
{
    ( m_workingArea->getViewerSelected()->getViewer() )->verticalFlip();
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
    foreach( Study *study, m_patient->getStudies() )
    {
        foreach( Series *series, study->getSeries() )
        {
            foreach( Volume *volume, series->getVolumesList() )
            {
                connect( volume, SIGNAL(progress(int)), SLOT( updateVolumeLoadProgressNotification(int) ) );
            }
        }
    }
}

void Q2DViewerExtension::updateVolumeLoadProgressNotification(int progress)
{
    m_progressDialog->setValue(progress);
    qApp->processEvents();
}

void Q2DViewerExtension::createProgressDialog()
{
    m_progressDialog = new QProgressDialog( this );
    m_progressDialog->setModal( false );
    m_progressDialog->setRange( 0 , 100 );
    m_progressDialog->setMinimumDuration( 0 );
    m_progressDialog->setWindowTitle( tr("Loading") );
    m_progressDialog->setLabelText( tr("Loading data, please wait...") );
    m_progressDialog->setCancelButton( 0 );
}

void Q2DViewerExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->getToolAction("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_referenceLinesToolButton->setDefaultAction( m_toolManager->getToolAction("ReferenceLinesTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );
    m_polylineButton->setDefaultAction( m_toolManager->getToolAction( "PolylineROITool" ) );
    m_distanceToolButton->setDefaultAction( m_toolManager->getToolAction( "DistanceTool" ) );
    m_eraserToolButton->setDefaultAction( m_toolManager->getToolAction( "EraserTool" ) );
    m_cursor3DToolButton->setDefaultAction( m_toolManager->getToolAction("Cursor3DTool") );
    m_angleToolButton->setDefaultAction( m_toolManager->getToolAction( "AngleTool" ) );

    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // Tool d'slicing per teclat
    QAction *slicingKeyboardTool = m_toolManager->getToolAction("SlicingKeyboardTool");
    slicingKeyboardTool->trigger();

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "DistanceTool" << "EraserTool" << "ScreenShotTool"<< "AngleTool" << "Cursor3DTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_translateToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    // La tool de sincronització sempre estarà activada, encara que no hi tingui cap visualitzador
    m_toolManager->getToolAction("SynchronizeTool")->setChecked( true );

    //TODO de moment fem exclusiu la tool de sincronització i la de cursor 3d manualment perque la
    //sincronització no té el model de totes les tools
    connect( m_cursor3DToolButton->defaultAction() , SIGNAL( triggered() ) , SLOT( disableSynchronization() ) );
}

void Q2DViewerExtension::initializeDefaultTools( Q2DViewer *viewer )
{
    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "ReferenceLinesTool" << "TranslateTool" << "WindowLevelTool" << "ScreenShotTool" << "WindowLevelPresetsTool" << "PolylineROITool" << "DistanceTool" << "SlicingKeyboardTool" << "EraserTool" << "AngleTool" << "Cursor3DTool";

    m_toolManager->setViewerTools( viewer, toolsList );
}

void Q2DViewerExtension::activateNewViewer( Q2DViewerWidget * newViewerWidget)
{

     // i si cal, activem les annotacions
    if( m_viewerInformationToolButton->isChecked() )
        newViewerWidget->getViewer()->enableAnnotation( Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation |
        Q2DViewer::RulersAnnotation | Q2DViewer::SliceAnnotation | Q2DViewer::PatientInformationAnnotation |
        Q2DViewer::AcquisitionInformationAnnotation, true );

    connect( newViewerWidget, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );

    initializeDefaultTools( newViewerWidget->getViewer() );
}

void Q2DViewerExtension::changeSelectedViewer( Q2DViewerWidget * viewerWidget )
{
    if( !viewerWidget )
    {
        DEBUG_LOG("El Viewer donat és NUL!");
        return;
    }

    if ( viewerWidget != m_viewer )
    {
        ///TODO canviar aquestes connexions i desconnexions per dos mètodes el qual
        /// enviin el senyal al visualitzador que toca.
        if( m_viewer )
        {
//             disconnect( m_predefinedSlicesGrid , SIGNAL( selectedGrid( int , int ) ) , m_workingArea->getViewerSelected()->getViewer(), SLOT( setGrid( int, int ) ) );
//             disconnect( m_sliceTableGrid , SIGNAL( selectedGrid( int , int ) ) , m_workingArea->getViewerSelected()->getViewer(), SLOT( setGrid( int, int ) ) );
            disconnect( m_viewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), this, SLOT( validePhases() ) );
            disconnect( m_viewer->getViewer(), SIGNAL( viewChanged(int) ), this, SLOT( updateDICOMInformationButton(int) ) );
        }
        m_viewer = viewerWidget;
        validePhases();

//         connect( m_predefinedSlicesGrid, SIGNAL( selectedGrid( int , int ) ) , m_workingArea->getViewerSelected()->getViewer(), SLOT( setGrid( int, int ) ) );
//         connect( m_sliceTableGrid, SIGNAL( selectedGrid( int , int ) ) , m_workingArea->getViewerSelected()->getViewer(), SLOT( setGrid( int, int ) ) );
        connect( m_viewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );
        connect( m_viewer->getViewer(), SIGNAL( viewChanged(int) ), SLOT( updateDICOMInformationButton(int) ) );

        // TODO potser hi hauria alguna manera més elegant, com tenir un slot a WindowLevelPresetsToolData
        // que es digués activateCurrentPreset() i el poguéssim connectar a algun signal
        m_windowLevelComboBox->setPresetsData( m_viewer->getViewer()->getWindowLevelData() );
        m_windowLevelComboBox->selectPreset( m_viewer->getViewer()->getWindowLevelData()->getCurrentPreset() );

        m_cineController->setQViewer( m_viewer->getViewer() );
        m_thickSlabWidget->link( m_viewer->getViewer() );
        updateDICOMInformationButton( m_viewer->getViewer()->getView() );
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
    QSettings settings;
    m_profile = settings.value("Starviewer-App-2DViewer/profile").toString();
}

void Q2DViewerExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
}

void Q2DViewerExtension::synchronization( Q2DViewerWidget * viewer, bool active )
{
    if( active )
    {
        // Per defecte sincronitzem només la tool de slicing
        ToolConfiguration * synchronizeConfiguration = new ToolConfiguration();
        synchronizeConfiguration->addAttribute( "Slicing", QVariant( true ) );
        m_toolManager->setViewerTool( viewer->getViewer(), "SynchronizeTool", synchronizeConfiguration );
        m_toolManager->activateTool("SynchronizeTool");

        //TODO si el cursor 3d està seleccionat, el deseleccionem. Solució temporal
        if( m_cursor3DToolButton->isChecked () )
            m_slicingToolButton->defaultAction()->setChecked( true );
    }
    else
    {
        m_toolManager->removeViewerTool( viewer->getViewer(), "SynchronizeTool" );
    }
}

void Q2DViewerExtension::disableSynchronization()
{
    //TODO Mètode per desactivar l'eina i el boto de sincronització dels visualitzadors quan
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

}
