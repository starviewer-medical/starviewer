/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "thickslabextension.h"
#include "toolmanager.h"
#include "volume.h"
#include "image.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "toolsactionfactory.h"
#include "qthickslabwidget.h"
#include "../../main/q2dviewer/menugridwidget.h"
#include "../../main/q2dviewer/tablemenu.h"
#include "patient.h"
#include "study.h"
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
// VTK
#include <vtkRenderer.h>

namespace udg {

ThickSlabExtension::ThickSlabExtension( QWidget *parent )
 : QWidget( parent ), m_presentationStateAttacher(0)
{
    setupUi( this );

    //per a la release 0.4 les ROI no es podran utilitzar TODO ocultem fins que funcionin correctament com volem
    m_roiToolButton->setVisible( false );
    // TODO deshabilitem els presentation states fins la release en què es tornin a habilitar
    m_presentationStateSwitchToolButton->setVisible(false);

    m_mainVolume = 0;
    m_keyImageNoteAttacher = NULL;
    m_keyImageNote = NULL;

    m_patient = NULL;
    m_selectedViewer = new QThickSlabWidget( m_workingArea );
    m_selectedViewer->getViewer()->render();

    m_predefinedSeriesGrid = new MenuGridWidget();
    m_seriesTableGrid = new TableMenu();
    m_predefinedSlicesGrid = new MenuGridWidget();
    m_sliceTableGrid = new TableMenu();

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

ThickSlabExtension::~ThickSlabExtension()
{
    writeSettings();
    delete m_toolManager;
}

void ThickSlabExtension::createActions()
{
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("&Axial View") );
    m_axialViewAction->setShortcut( tr("Ctrl+A") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    m_axialViewToolButton->setDefaultAction( m_axialViewAction );

    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("&Sagital View") );
    m_sagitalViewAction->setShortcut( tr("Ctrl+S") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Saggital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );

    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("&Coronal View") );
    m_coronalViewAction->setShortcut( tr("Ctrl+C") );
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

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ), SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ), SLOT( rotateCounterClockWise() ) );

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
    m_distanceToolButton->setDefaultAction( m_distanceAction );

    m_roiAction = m_actionFactory->getActionFrom( "ROITool" );
    m_roiToolButton->setDefaultAction( m_roiAction );

    connect( m_actionFactory , SIGNAL( triggeredTool( QString ) ) , m_selectedViewer->getViewer() , SLOT( setTool( QString ) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );

    m_toolsActionGroup->addAction( m_distanceAction );
    m_toolsActionGroup->addAction( m_roiAction );
}

void ThickSlabExtension::enablePresentationState(bool enable)
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

void ThickSlabExtension::createConnections()
{
    // adicionals, \TODO ara es fa "a saco" perÃ² s'ha de millorar
    connect( m_axialViewAction , SIGNAL( triggered() ), SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ), SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ), SLOT( changeViewToCoronal() ) );

    // Menus
    connect( m_downButtonGrid , SIGNAL( clicked ( bool ) ), SLOT( showPredefinedGrid() ) );
    connect( m_buttonGrid , SIGNAL( clicked ( bool ) ), SLOT( showInteractiveTable() ) );
    connect( m_downImageGrid , SIGNAL( clicked ( bool ) ), SLOT( showPredefinedImageGrid() ) );
    connect( m_imageGrid , SIGNAL( clicked ( bool ) ), SLOT( showInteractiveImageTable() ) );

    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ), SLOT( setWindowLevel(double,double) ) );

    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ), SLOT( resetWindowLevelToDefault() ) );

    // Connexions del menu
    connect( m_predefinedSeriesGrid , SIGNAL( selectedGrid( int , int ) ), SLOT( setGrid( int, int ) ) );
    connect( m_seriesTableGrid , SIGNAL( selectedGrid( int , int ) ), SLOT( setGrid( int, int ) ) );

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), SLOT( enablePresentationState(bool) ) );

    // Connexions necessaries pel primer visualitzador
    connect( m_selectedViewer , SIGNAL( selected( QThickSlabWidget * ) ), SLOT( setViewerSelected( QThickSlabWidget * ) ) );
    connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume * ) ), SLOT( validePhases() ) );

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_volumeInformation , SIGNAL( stateChanged ( int ) ), SLOT( showInformation( int ) ) );

    connect( m_slabThicknessSpinBox, SIGNAL( valueChanged(double) ), m_selectedViewer, SLOT( setSlabThickness(double) ) );
}

void ThickSlabExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_vectorViewers.value( 0 )->setInput( m_mainVolume );
    validePhases();

    INFO_LOG("ThickSlabExtension: Donem l'input principal")
    changeViewToAxial();
}

void ThickSlabExtension::changeViewToAxial()
{
    m_currentView = Axial;
    m_selectedViewer->changeViewToAxial();
}

void ThickSlabExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    m_selectedViewer->changeViewToSagital();
}

void ThickSlabExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    m_selectedViewer->changeViewToCoronal();
}

void ThickSlabExtension::setView( ViewType view )
{
    switch( view )
    {
    case Axial:
        changeViewToAxial();
    break;
    case Sagital:
        changeViewToSagital();
    break;
    case Coronal:
        changeViewToCoronal();
    break;
    }
}

void ThickSlabExtension::loadKeyImageNote(const QString &filename)
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

void ThickSlabExtension::loadPresentationState(const QString &filename)
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

void ThickSlabExtension::initLayouts()
{
    m_rows = 1;
    m_columns = 1;
    m_totalRows = 1;
    m_totalColumns = 1;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    m_gridLayout->setMargin(0);

    m_verticalLayout = new QVBoxLayout();
    m_verticalLayout->setSpacing(0);
    m_verticalLayout->setMargin(0);

    QHBoxLayout *horizontal = new QHBoxLayout();
    horizontal->setSpacing(0);
    horizontal->setMargin(0);

    horizontal->addWidget( m_selectedViewer );
    m_verticalLayout->addLayout( horizontal,0 );
    m_gridLayout->addLayout( m_verticalLayout,0,0 );

    m_qHorizontalLayoutVector.push_back( horizontal );
    m_vectorViewers.push_back( m_selectedViewer );
    m_workingArea->setLayout(m_gridLayout);

    m_selectedViewer->setSelected( true );
}

void ThickSlabExtension::addColumns( int columns )
{
    QVector<QHBoxLayout*>::Iterator it;
    int posViewer = m_columns;
    QThickSlabWidget * newViewer;

    while( columns > 0 )
    {
        it = m_qHorizontalLayoutVector.begin();
        m_columns += 1;
        m_totalColumns += 1;
        // Afegim un widget a cada fila per tenir una columna més
        int i = 0;
        while( it != m_qHorizontalLayoutVector.end() )
        {
            newViewer = getNewQThickSlabWidget();
            (*it)->addWidget( newViewer );
            m_vectorViewers.insert( posViewer,newViewer );
            posViewer += m_columns;
            it++;
            if( i >= m_rows ) newViewer->hide();
            i++;

        }
        posViewer = m_columns;
        columns--;
    }
}

void ThickSlabExtension::addRows( int rows )
{
    QHBoxLayout *horizontal;
    QThickSlabWidget *newViewer;
    int i;

    while( rows > 0 )
    {
        horizontal = new QHBoxLayout();
        m_verticalLayout->addLayout( horizontal,0 );
        m_qHorizontalLayoutVector.push_back( horizontal );
        m_rows += 1;
        m_totalRows += 1;
        //Afegim tants widgets com columnes
        for(i = 0; i < m_totalColumns; i++)
        {
            newViewer = getNewQThickSlabWidget();
            horizontal->addWidget( newViewer );
            m_vectorViewers.push_back( newViewer );
            if( i >= m_columns) newViewer->hide();
        }
        rows--;
    }
}

void ThickSlabExtension::removeColumns( int columns )
{
    QVector<QHBoxLayout*>::Iterator it = m_qHorizontalLayoutVector.begin();
    int posViewer = m_columns-1;
    QThickSlabWidget * oldViewer;

    while( columns > 0 && m_columns > 1 )
    {
        it = m_qHorizontalLayoutVector.begin();
        // Eliminem un widget de cada fila per tenir una columna menys
        while (it != m_qHorizontalLayoutVector.end())
        {
            oldViewer = m_vectorViewers.value(posViewer);
            ( *it )->removeWidget( oldViewer );
            m_vectorViewers.remove( posViewer );
            if ( m_selectedViewer == oldViewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer += (m_columns-1);
            it++;
        }
        m_columns--;
        posViewer = m_columns-1;
        columns--;
    }
}

void ThickSlabExtension::removeRows( int rows )
{
    int i;
    m_verticalLayout->removeItem(m_verticalLayout->itemAt(m_verticalLayout->count()));
    int posViewer = m_vectorViewers.count()-1;
    QThickSlabWidget * oldViewer;

    while( rows > 0 && m_rows > 1 )
    {
         m_qHorizontalLayoutVector.pop_back();
        //Eliminem tants widgets com columnes
        for(i = 0; i < m_columns; i++)
        {
            oldViewer = m_vectorViewers.value(posViewer);
            m_vectorViewers.remove(posViewer);
            if ( m_selectedViewer == oldViewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
            delete oldViewer;
            posViewer -= 1;
        }
        m_rows--;
        rows--;
    }
}

QThickSlabWidget* ThickSlabExtension::getNewQThickSlabWidget()
{
    QThickSlabWidget *newViewer = new QThickSlabWidget( m_workingArea );
    newViewer->getViewer()->render();
    (newViewer->getViewer() )->setTool( (m_vectorViewers.value( 0 )->getViewer() )->getCurrentToolName() );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , newViewer->getViewer(), SLOT( setTool(QString) ) );
    connect( newViewer , SIGNAL( selected( QThickSlabWidget * ) ), SLOT( setViewerSelected( QThickSlabWidget * ) ) );

    int state = m_volumeInformation->checkState();

    if( state == Qt::Unchecked )
    {
        newViewer->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );
    }
    else if( state == Qt::Checked )
    {
        newViewer->getViewer()->enableAnnotation( Q2DViewer::AllAnnotation, true );
    }

    return newViewer;
}

void ThickSlabExtension::setGrid( int rows, int columns )
{
    // Mirem si les tenim amagades i mostrem totes les necessaries
    int windowsToShow = 0;
    int windowsToCreate = 0;
    int windowsToHide = 0;

    if( rows > m_rows )
    {
        int hideWindows = m_totalRows - m_rows;

        if( hideWindows < (rows - m_rows) ) windowsToShow = hideWindows;
        else windowsToShow = rows-m_rows;
        showRows( windowsToShow );

        if( rows > m_totalRows ) windowsToCreate = rows - m_totalRows;
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

        if( hideWindows < (columns - m_columns) ) windowsToShow = hideWindows;
        else windowsToShow = columns-m_columns;
        showColumns( windowsToShow );

        if( columns > m_totalColumns ) windowsToCreate = columns - m_totalColumns;
        addColumns( windowsToCreate );
    }
    else if( columns < m_columns )
    {
        hideColumns( m_columns - columns );
    }
}

void ThickSlabExtension::setViewerSelected( QThickSlabWidget * viewer )
{
    if ( viewer != m_selectedViewer )
    {
        ///TODO canviar aquestes connexions i desconnexions per dos mètodes el qual
        /// enviin el senyal al visualitzador que toca.
        disconnect( m_predefinedSlicesGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        disconnect( m_sliceTableGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        disconnect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume * ) ), this, SLOT( validePhases() ) );

        m_selectedViewer->setSelected( false );
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected( true );
        validePhases();

        connect( m_predefinedSlicesGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        connect( m_sliceTableGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->getViewer(), SLOT( setGrid( int, int ) ) );
        connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume * ) ), SLOT( validePhases() ) );
    }
}

void ThickSlabExtension::rotateClockWise()
{
    ( m_selectedViewer->getViewer() )->rotateClockWise();
}

void ThickSlabExtension::rotateCounterClockWise()
{
    ( m_selectedViewer->getViewer() )->rotateCounterClockWise();
}

void ThickSlabExtension::horizontalFlip()
{
    ( m_selectedViewer->getViewer() )->horizontalFlip();
}

void ThickSlabExtension::verticalFlip()
{
    ( m_selectedViewer->getViewer() )->verticalFlip();
}

void ThickSlabExtension::setWindowLevel(double wl1 ,double wl2)
{
    ( m_selectedViewer->getViewer() )->setWindowLevel( wl1, wl2 );
}

void ThickSlabExtension::resetWindowLevelToDefault()
{
    ( m_selectedViewer->getViewer() )->resetWindowLevelToDefault();
}

void ThickSlabExtension::showPredefinedGrid()
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

void ThickSlabExtension::showInteractiveTable()
{
    QPoint point = m_buttonGrid->mapToGlobal( QPoint(0,0) );
    m_seriesTableGrid->move( point.x(),( point.y() + m_buttonGrid->frameGeometry().height() ) );
    m_seriesTableGrid->show();
}

void ThickSlabExtension::showPredefinedImageGrid()
{
    QPoint point = m_imageGrid->mapToGlobal( QPoint(0,0) );
    m_predefinedSlicesGrid->move( point.x(),( point.y() + m_imageGrid->frameGeometry().height() ) );
    m_predefinedSlicesGrid->createPredefinedGrids( m_selectedViewer->getViewer()->getNumberOfSlices() );
    m_predefinedSlicesGrid->show();
}

void ThickSlabExtension::showInteractiveImageTable()
{
    QPoint point = m_imageGrid->mapToGlobal( QPoint(0,0) );
    m_sliceTableGrid->move( point.x(),( point.y() + m_imageGrid->frameGeometry().height() ) );
    m_sliceTableGrid->show();
}

Patient* ThickSlabExtension::getPatient() const
{
    return m_patient;
}

void ThickSlabExtension::setPatient( Patient *patient )
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

void ThickSlabExtension::updateVolumeLoadProgressNotification(int progress)
{
    m_progressDialog->setValue(progress);
}

void ThickSlabExtension::createProgressDialog()
{
    m_progressDialog = new QProgressDialog( this );
    m_progressDialog->setModal( false );
    m_progressDialog->setRange( 0 , 100 );
    m_progressDialog->setMinimumDuration( 0 );
    m_progressDialog->setWindowTitle( tr("Loading") );
    m_progressDialog->setLabelText( tr("Loading data, please wait...") );
    m_progressDialog->setCancelButton( 0 );
}

void ThickSlabExtension::showRows( int rows )
{
    QThickSlabWidget *viewer;
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

void ThickSlabExtension::hideRows( int rows )
{
    QThickSlabWidget *viewer;
    int numColumn;

    while( rows > 0 )
    {
        m_rows--;
        for( numColumn = 0; numColumn < m_columns; numColumn++ )
        {
            viewer = m_vectorViewers.value( ( ( m_totalColumns*m_rows ) + numColumn ) );
            viewer->hide();
            if ( m_selectedViewer == viewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        rows--;
    }
}

void ThickSlabExtension::showColumns( int columns )
{
    QThickSlabWidget *viewer;
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

void ThickSlabExtension::hideColumns( int columns )
{
    QThickSlabWidget *viewer;
    int numRow;

    while( columns > 0 )
    {
        m_columns--;
        for( numRow = 0; numRow < m_rows; numRow++ )
        {
            viewer = m_vectorViewers.value( ( m_totalColumns*numRow ) + m_columns );
            viewer->hide();
            if ( m_selectedViewer == viewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        columns--;
    }
}

void ThickSlabExtension::showInformation( int state )
{
    int numViewer;

    if( state == Qt::Unchecked )
    {
        for( numViewer = 0; numViewer < m_vectorViewers.size(); numViewer++ )
        {
            m_vectorViewers.value( numViewer )->getViewer()->removeAnnotation( Q2DViewer::AllAnnotation );
        }
    }
    else if( state == Qt::Checked )
    {
        for( numViewer = 0; numViewer < m_vectorViewers.size(); numViewer++ )
        {
            m_vectorViewers.value( numViewer )->getViewer()->enableAnnotation( Q2DViewer::AllAnnotation, true );
        }
    }
}

void ThickSlabExtension::validePhases()
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

void ThickSlabExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
}

void ThickSlabExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-2DViewer");

    settings.endGroup();
}

void ThickSlabExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->getToolAction("SlicingTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );

    // definim els grups exclusius
    QStringList exclusiveTools;
    exclusiveTools << "ZoomTool" << "SlicingTool";
    m_toolManager->addExclusiveToolsGroup("Group1", exclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_slicingToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_windowLevelToolButton->defaultAction()->trigger();

    // registrem al manager les tools que van amb el viewer principal
    initializeDefaultTools( m_selectedViewer->getViewer() );

    /// \todo Comentat perquè no existeix l'slot.
//     connect( m_selectedViewer, SIGNAL( sincronize( QThickSlabWidget *, bool ) ), SLOT( sincronization( QThickSlabWidget *, bool ) ) );
}

void ThickSlabExtension::initializeDefaultTools( Q2DViewer *viewer )
{
    QStringList toolsList;
    toolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "ScreenShotTool";
    m_toolManager->setViewerTools( viewer, toolsList );
    m_toolManager->refreshConnections();
}

}
