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
#include "toolsactionfactory.h"
#include "q2dviewerwidget.h"
#include "menugridwidget.h"
#include "tablemenu.h"
#include "patient.h"
#include "study.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "windowlevelpresetstooldata.h"
#include "qdicomdump.h"

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
 : QWidget( parent ), m_presentationStateAttacher(0)
{
    setupUi( this );

    //per a la release 0.4 les ROI no es podran utilitzar TODO ocultem fins que funcionin correctament com volem
    m_roiToolButton->setVisible( false );
    m_oldDistanceToolButton->setVisible( false );
    m_rotateCounterClockWiseToolButton->setVisible( false );
    m_flipVerticalToolButton->setVisible( false );
    m_flipHorizontalToolButton->setVisible( false );
    m_voxelInformationToolButton->setVisible( false );
    // TODO deshabilitem els presentation states fins la release en què es tornin a habilitar
    m_presentationStateSwitchToolButton->setVisible(false);

    m_mainVolume = 0;
    m_keyImageNoteAttacher = NULL;
    m_keyImageNote = NULL;

    m_patient = NULL;

    // TODO estem duplicant codi que es posa a setViewerSelected. Caldria fer algo per unificar-ho i que estigui més consistent
    m_selectedViewer = new Q2DViewerWidget( m_workingArea );
    m_thickSlabWidget->link( m_selectedViewer->getViewer() );
    // activem les dades de ww/wl de la combo box
    m_windowLevelComboBox->setPresetsData( m_selectedViewer->getViewer()->getWindowLevelData() );
    m_windowLevelComboBox->selectPreset( m_selectedViewer->getViewer()->getWindowLevelData()->getCurrentPreset() );
    connect( m_selectedViewer->getViewer(), SIGNAL( viewChanged(int) ), SLOT( updateDICOMInformationButton(int) ) );

    m_predefinedSeriesGrid = new MenuGridWidget();
    m_seriesTableGrid = new TableMenu();
    m_predefinedSlicesGrid = new MenuGridWidget();
    m_sliceTableGrid = new TableMenu();
    m_dicomDumpCurrentDisplayedImage = new QDicomDump();

    createProgressDialog();
    readSettings();
    createActions();
    createConnections();
    initLayouts();
    // TODO de moment no fem accessible aquesta funcionalitat ja que no està a punt
    m_imageGrid->setVisible(false);
    m_downImageGrid->setVisible(false);
    initializeTools();
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();
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
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
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

    // Tools
    m_actionFactory = new ToolsActionFactory( 0 );
    m_distanceAction = m_actionFactory->getActionFrom( "DistanceTool" );
    m_oldDistanceToolButton->setDefaultAction( m_distanceAction );

    m_roiAction = m_actionFactory->getActionFrom( "ROITool" );
    m_roiToolButton->setDefaultAction( m_roiAction );

    connect( m_actionFactory , SIGNAL( triggeredTool( QString ) ) , m_selectedViewer->getViewer() , SLOT( setTool( QString ) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_distanceAction );
    m_toolsActionGroup->addAction( m_roiAction );
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
    connect( m_predefinedSeriesGrid, SIGNAL( selectedGrid( int , int ) ), SLOT( setGrid( int, int ) ) );
    connect( m_seriesTableGrid, SIGNAL( selectedGrid( int , int ) ), SLOT( setGrid( int, int ) ) );

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), SLOT( enablePresentationState(bool) ) );

    // Connexions necessaries pel primer visualitzador
    connect( m_selectedViewer, SIGNAL( selected( Q2DViewerWidget *) ), SLOT( setViewerSelected( Q2DViewerWidget *) ) );
    connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_viewerInformationToolButton, SIGNAL( toggled( bool ) ), SLOT( showViewerInformation( bool ) ) );

    connect( m_dicomDumpToolButton, SIGNAL( clicked() ) , SLOT( showDicomDumpCurrentDisplayedImage() ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_vectorViewers.value( 0 )->setInput( m_mainVolume );
    validePhases();
    m_cineController->setQViewer( m_vectorViewers.value(0)->getViewer() );
    INFO_LOG("Q2DViewerExtension: Donem l'input principal");
}

void Q2DViewerExtension::resetViewToAxial()
{
    m_selectedViewer->resetViewToAxial();
}

void Q2DViewerExtension::resetViewToSagital()
{
    m_selectedViewer->resetViewToSagital();
}

void Q2DViewerExtension::resetViewToCoronal()
{
    m_selectedViewer->resetViewToCoronal();
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
    m_keyImageNoteAttacher = new Q2DViewerKeyImageNoteAttacher( m_vectorViewers.value( 0 )->getViewer(), m_keyImageNote );
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
    m_presentationStateAttacher = new Q2DViewerPresentationStateAttacher( m_vectorViewers.value( 0 )->getViewer(), qPrintable(filename) );
    m_presentationStateAction->setEnabled( true );
    m_presentationStateAction->setChecked( true );
}

void Q2DViewerExtension::initLayouts()
{
    m_rows = 1;
    m_columns = 1;
    m_totalRows = 1;
    m_totalColumns = 1;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    m_gridLayout->setMargin(0);

    m_viewersLayout = new QGridLayout();
    m_viewersLayout->setSpacing(0);
    m_viewersLayout->setMargin(0);

    m_viewersLayout->addWidget( m_selectedViewer, 0, 0 );
    m_gridLayout->addLayout( m_viewersLayout, 0, 0 );

    m_vectorViewers.push_back( m_selectedViewer );
    m_workingArea->setLayout(m_gridLayout);

    m_selectedViewer->setSelected( true );
}

void Q2DViewerExtension::addColumns( int columns )
{
    int posViewer = m_columns;
    Q2DViewerWidget *newViewer;

    int rows;
    while( columns > 0 )
    {
        rows = 0;
        m_columns += 1;
        m_totalColumns += 1;
        while( rows < m_viewersLayout->rowCount() )
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget( newViewer, rows, m_totalColumns-1);
            m_vectorViewers.insert( posViewer,newViewer );
            initializeDefaultTools( newViewer->getViewer() );
            posViewer += m_columns;
            if( rows >= m_rows )
                newViewer->hide();
            rows++;
        }
        posViewer = m_columns;
        columns--;
    }
}

void Q2DViewerExtension::addRows( int rows )
{
    Q2DViewerWidget *newViewer;
    int column;

    while( rows > 0 )
    {
        m_rows += 1;
        m_totalRows += 1;
        //Afegim tants widgets com columnes
        for(column = 0; column < m_totalColumns; column++)
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget( newViewer, m_rows-1, column);
            m_vectorViewers.push_back( newViewer );
            initializeDefaultTools( newViewer->getViewer() );
            if( column >= m_columns)
                newViewer->hide();
        }
        rows--;
    }
}

void Q2DViewerExtension::removeColumns( int columns )
{
    int posViewer = m_columns-1;
    Q2DViewerWidget *oldViewer;

    int rows;
    while( columns > 0 && m_columns > 1 )
    {
        rows = 0;
        // Eliminem un widget de cada fila per tenir una columna menys
        while (rows < m_viewersLayout->rowCount() )
        {
            oldViewer = m_vectorViewers.value(posViewer);
            m_viewersLayout->removeWidget( oldViewer );
            m_vectorViewers.remove( posViewer );
            if ( m_selectedViewer == oldViewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer += (m_columns-1);
            rows++;
        }
        m_columns--;
        posViewer = m_columns-1;
        columns--;
    }
}

void Q2DViewerExtension::removeRows( int rows )
{
    int i;
    int posViewer = m_vectorViewers.count()-1;
    Q2DViewerWidget *oldViewer;

    while( rows > 0 && m_rows > 1 )
    {
        //Eliminem tants widgets com columnes
        for(i = 0; i < m_columns; i++)
        {
            oldViewer = m_vectorViewers.value(posViewer);
            m_vectorViewers.remove(posViewer);
            m_viewersLayout->removeWidget( oldViewer );
            // TODO eliminar els viewers que treiem del toolManager???
            if ( m_selectedViewer == oldViewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer -= 1;
        }
        m_rows--;
        rows--;
    }
}

Q2DViewerWidget* Q2DViewerExtension::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget( m_workingArea );
    (newViewer->getViewer() )->setTool( (m_vectorViewers.value( 0 )->getViewer() )->getCurrentToolName() );
    connect( m_actionFactory, SIGNAL( triggeredTool(QString) ) , newViewer->getViewer(), SLOT( setTool(QString) ) );
    connect( newViewer, SIGNAL( selected( Q2DViewerWidget *) ), SLOT( setViewerSelected( Q2DViewerWidget *) ) );

    if( m_viewerInformationToolButton->isChecked() )
        newViewer->getViewer()->enableAnnotation( Q2DViewer::AllAnnotation, true );
    else
        newViewer->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );

    connect( newViewer, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );

    return newViewer;
}

void Q2DViewerExtension::setGrid( int rows, int columns )
{
    // Mirem si les tenim amagades i mostrem totes les necessaries
    int windowsToShow = 0;
    int windowsToCreate = 0;
    int windowsToHide = 0;

    if( rows > m_rows )
    {
        int hideWindows = m_totalRows - m_rows;

        if( hideWindows < (rows - m_rows) )
             windowsToShow = hideWindows;
        else
            windowsToShow = rows-m_rows;

        showRows( windowsToShow );

        if( rows > m_totalRows )
            windowsToCreate = rows - m_totalRows;

        addRows( windowsToCreate );
    }
    else if( rows < m_rows )
    {
        hideRows( m_rows - rows );
    }

    windowsToShow = 0;
    windowsToCreate = 0;
    windowsToHide = 0;

    if( columns > m_columns )
    {
        int hideWindows = m_totalColumns - m_columns;

        if( hideWindows < (columns - m_columns) )
            windowsToShow = hideWindows;
        else
            windowsToShow = columns-m_columns;

        showColumns( windowsToShow );

        if( columns > m_totalColumns )
            windowsToCreate = columns - m_totalColumns;

        addColumns( windowsToCreate );
    }
    else if( columns < m_columns )
    {
        hideColumns( m_columns - columns );
    }
}

void Q2DViewerExtension::setViewerSelected( Q2DViewerWidget *viewer )
{
    if ( viewer != m_selectedViewer )
    {
        ///TODO canviar aquestes connexions i desconnexions per dos mètodes el qual
        /// enviin el senyal al visualitzador que toca.
        disconnect( m_predefinedSlicesGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        disconnect( m_sliceTableGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        disconnect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), this, SLOT( validePhases() ) );
        disconnect( m_selectedViewer->getViewer(), SIGNAL( viewChanged(int) ), this, SLOT( updateDICOMInformationButton(int) ) );

        m_selectedViewer->setSelected( false );
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected( true );
        validePhases();

        connect( m_predefinedSlicesGrid, SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        connect( m_sliceTableGrid, SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume *) ), SLOT( validePhases() ) );
        connect( m_selectedViewer->getViewer(), SIGNAL( viewChanged(int) ), SLOT( updateDICOMInformationButton(int) ) );

        // TODO potser hi hauria alguna manera més elegant, com tenir un slot a WindowLevelPresetsToolData
        // que es digués activateCurrentPreset() i el poguéssim connectar a algun signal
        m_windowLevelComboBox->setPresetsData( m_selectedViewer->getViewer()->getWindowLevelData() );
        m_windowLevelComboBox->selectPreset( m_selectedViewer->getViewer()->getWindowLevelData()->getCurrentPreset() );

        m_cineController->setQViewer( viewer->getViewer() );
        m_thickSlabWidget->link( m_selectedViewer->getViewer() );
        updateDICOMInformationButton( m_selectedViewer->getViewer()->getView() );
    }
}

void Q2DViewerExtension::rotateClockWise()
{
    ( m_selectedViewer->getViewer() )->rotateClockWise();
}

void Q2DViewerExtension::rotateCounterClockWise()
{
    ( m_selectedViewer->getViewer() )->rotateCounterClockWise();
}

void Q2DViewerExtension::horizontalFlip()
{
    ( m_selectedViewer->getViewer() )->horizontalFlip();
}

void Q2DViewerExtension::verticalFlip()
{
    ( m_selectedViewer->getViewer() )->verticalFlip();
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
    m_predefinedSlicesGrid->createPredefinedGrids( m_selectedViewer->getViewer()->getMaximumSlice() );
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
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->getToolAction("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );
    m_polylineButton->setDefaultAction( m_toolManager->getToolAction( "PolylineROITool" ) );
    m_distanceToolButton->setDefaultAction( m_toolManager->getToolAction( "DistanceTool" ) );
    m_eraserToolButton->setDefaultAction( m_toolManager->getToolAction( "EraserTool" ) );

    // activem l'eina de valors predefinits de window level
    QAction *windowLevelPresetsTool = m_toolManager->getToolAction("WindowLevelPresetsTool");
    windowLevelPresetsTool->trigger();

    // Tool d'slicing per teclat
    QAction *slicingKeyboardTool = m_toolManager->getToolAction("SlicingKeyboardTool");
    slicingKeyboardTool->trigger();

    // definim els grups exclusius
    QStringList exclusiveTools;
    exclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "DistanceTool" << "EraserTool";
    m_toolManager->addExclusiveToolsGroup("Group1", exclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_translateToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    // La tool de sincronització sempre estarà activada, encara que no hi tingui cap visualitzador
    m_toolManager->getToolAction("SynchronizeTool")->setChecked( true );

    // registrem al manager les tools que van amb el viewer principal
    initializeDefaultTools( m_selectedViewer->getViewer() );

    connect( m_selectedViewer, SIGNAL( synchronize( Q2DViewerWidget *, bool ) ), SLOT( synchronization( Q2DViewerWidget *, bool ) ) );
}

void Q2DViewerExtension::initializeDefaultTools( Q2DViewer *viewer )
{
    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "ReferenceLinesTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool" << "WindowLevelPresetsTool" << "PolylineROITool" << "DistanceTool" << "SlicingKeyboardTool" << "EraserTool";
    m_toolManager->setViewerTools( viewer, toolsList );
    m_toolManager->refreshConnections();
}

void Q2DViewerExtension::showRows( int rows )
{
    Q2DViewerWidget *viewer;
    int numColumn;

    while( rows > 0 )
    {
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*m_rows ) + numColumn );
            viewer->show();
        }
        m_rows++;
        rows--;
    }
}

void Q2DViewerExtension::hideRows( int rows )
{
    Q2DViewerWidget *viewer;
    int numColumn;

    while( rows > 0 )
    {
        m_rows--;
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            viewer = m_vectorViewers.value( ( ( m_totalColumns*m_rows ) + numColumn ) );
            viewer->hide();
            if ( m_selectedViewer == viewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        rows--;
    }
}

void Q2DViewerExtension::showColumns( int columns )
{
    Q2DViewerWidget *viewer;
    int numRow;

    while( columns > 0 )
    {
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*numRow ) + m_columns );
            viewer->show();
        }
        m_columns++;
        columns--;
    }
}

void Q2DViewerExtension::hideColumns( int columns )
{
    Q2DViewerWidget *viewer;
    int numRow;

    while( columns > 0 )
    {
        m_columns--;
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*numRow ) + m_columns );
            viewer->hide();
            if ( m_selectedViewer == viewer )
                setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        columns--;
    }
}

void Q2DViewerExtension::showViewerInformation( bool show )
{
    int numViewer;

    if( show )
    {
        for( numViewer = 0; numViewer < m_vectorViewers.size(); numViewer++ )
        {
            m_vectorViewers.value( numViewer )->getViewer()->enableAnnotation( Q2DViewer::AllAnnotation, true );
        }
    }
    else
    {
        for( numViewer = 0; numViewer < m_vectorViewers.size(); numViewer++ )
        {
            m_vectorViewers.value( numViewer )->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );
        }
    }
}

void Q2DViewerExtension::showDicomDumpCurrentDisplayedImage()
{
    m_dicomDumpCurrentDisplayedImage->setCurrentDisplayedImage( m_selectedViewer->getViewer()->getCurrentDisplayedImage() );
    m_dicomDumpCurrentDisplayedImage->show();
}

void Q2DViewerExtension::validePhases()
{
    if( m_selectedViewer->hasPhases() )
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
    if( view == Q2DViewer::Axial )
        m_dicomDumpToolButton->setEnabled(true);
    else
        m_dicomDumpToolButton->setEnabled(false);
}

void Q2DViewerExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
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
    }
    else
    {
        m_toolManager->removeViewerTool( viewer->getViewer(), "SynchronizeTool" );
    }
}

}
