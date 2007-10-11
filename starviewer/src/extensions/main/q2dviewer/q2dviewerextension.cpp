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
#include "slicing2dtool.h"

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
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
    m_selectedViewer = new Q2DViewerWidget( m_workingArea );
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
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();
}

void Q2DViewerExtension::createActions()
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
    // Pseudo-tool \TODO ara mateix no ho integrem dins del framework de tools, però potser que més endavant sí
    m_voxelInformationAction = new QAction( 0 );
    m_voxelInformationAction->setText( tr("Voxel Information") );
    m_voxelInformationAction->setShortcut( tr("Ctrl+I") );
    m_voxelInformationAction->setStatusTip( tr("Enable voxel information over cursor") );
    m_voxelInformationAction->setIcon( QIcon(":/images/voxelInformation.png") );
    m_voxelInformationAction->setCheckable( true );
    m_voxelInformationToolButton->setDefaultAction( m_voxelInformationAction );

    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , this , SLOT( setVoxelInformationCaptionEnabled(bool) ) );

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , this , SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ) , this , SLOT( rotateCounterClockWise() ) );

    m_flipHorizontalAction = new QAction(0);
    m_flipHorizontalAction->setText( tr("Flip Horizontal") );
    m_flipHorizontalAction->setStatusTip( tr("Flip the image horizontally") );
    m_flipHorizontalAction->setIcon( QIcon(":/images/flipHorizontal.png") );
    m_flipHorizontalToolButton->setDefaultAction( m_flipHorizontalAction );

    connect( m_flipHorizontalAction , SIGNAL( triggered() ) , this , SLOT( horizontalFlip() ) );

    m_flipVerticalAction = new QAction(0);
    m_flipVerticalAction->setText( tr("Flip Vertical") );
    m_flipVerticalAction->setStatusTip( tr("Flip the image vertically") );
    m_flipVerticalAction->setIcon( QIcon(":/images/flipVertical.png") );
    m_flipVerticalToolButton->setDefaultAction( m_flipVerticalAction );

    connect( m_flipVerticalAction , SIGNAL( triggered() ) , this , SLOT( verticalFlip() ) );

    // Tools
    m_actionFactory = new ToolsActionFactory( 0 );
    m_slicingAction = m_actionFactory->getActionFrom( "SlicingTool" );
    m_slicingToolButton->setDefaultAction( m_slicingAction );

    m_windowLevelAction = m_actionFactory->getActionFrom( "WindowLevelTool" );
    m_windowLevelToolButton->setDefaultAction( m_windowLevelAction );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = m_actionFactory->getActionFrom( "TranslateTool" );
    m_moveToolButton->setDefaultAction( m_moveAction );

    m_screenShotAction = m_actionFactory->getActionFrom( "ScreenShotTool" );
    m_screenShotToolButton->setDefaultAction( m_screenShotAction );

    m_distanceAction = m_actionFactory->getActionFrom( "DistanceTool" );
    m_distanceToolButton->setDefaultAction( m_distanceAction );

    m_roiAction = m_actionFactory->getActionFrom( "ROITool" );
    m_roiToolButton->setDefaultAction( m_roiAction );

    connect( m_actionFactory , SIGNAL( triggeredTool( QString ) ) , m_selectedViewer->getViewer() , SLOT( setTool( QString ) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_slicingAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_screenShotAction );
    m_toolsActionGroup->addAction( m_distanceAction );
    m_toolsActionGroup->addAction( m_roiAction );

    //activem per defecte una tool. \TODO podríem posar algun mecanisme especial per escollir la tool per defecte?
    m_slicingAction->trigger();
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
    // adicionals, \TODO ara es fa "a saco" perÃ² s'ha de millorar
    connect( m_axialViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToCoronal() ) );

    // Menus
    connect( m_downButtonGrid , SIGNAL( clicked ( bool ) ) , this , SLOT( showPredefinedGrid() ) );
    connect( m_buttonGrid , SIGNAL( clicked ( bool ) ) , this , SLOT( showInteractiveTable() ) );
    connect( m_downImageGrid , SIGNAL( clicked ( bool ) ) , this , SLOT( showPredefinedImageGrid() ) );
    connect( m_imageGrid , SIGNAL( clicked ( bool ) ) , this , SLOT( showInteractiveImageTable() ) );

    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , this , SLOT( setWindowLevel(double,double) ) );

    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , this , SLOT( resetWindowLevelToDefault() ) );

    // Connexions del menu
    connect( m_predefinedSeriesGrid , SIGNAL( selectedGrid( int , int ) ) , this, SLOT( setGrid( int, int ) ) );
    connect( m_seriesTableGrid , SIGNAL( selectedGrid( int , int ) ) , this, SLOT( setGrid( int, int ) ) );

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), this, SLOT( enablePresentationState(bool) ) );

    // Connexions necessaries pel primer visualitzador
    connect( m_selectedViewer , SIGNAL( selected( Q2DViewerWidget * ) ) , this, SLOT( setViewerSelected( Q2DViewerWidget * ) ) );
    connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume * ) ), this, SLOT( validePhases() ) ); 

    // mostrar o no la informacio del volum a cada visualitzador
    connect( m_volumeInformation , SIGNAL( stateChanged ( int ) ) , this, SLOT( showInformation( int ) ) );


}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_vectorViewers.value( 0 )->setInput( m_mainVolume );
    validePhases();

    // Omplim el combo amb tants window levels com tingui el volum
    int wlCount = m_mainVolume->getImages().at(0)->getNumberOfWindowLevels();
    if( wlCount )
    {
        for( int i = 0; i < wlCount; i++ )
        {
            QPair<double, double> windowLevel = m_mainVolume->getImages().at(0)->getWindowLevel( i );
            QString description = m_mainVolume->getImages().at(0)->getWindowLevelExplanation( i );
            if( !description.isEmpty() )
                m_windowLevelComboBox->insertWindowLevelPreset( windowLevel.first, windowLevel.second, i, description );
            else
                m_windowLevelComboBox->insertWindowLevelPreset( windowLevel.first, windowLevel.second, i, tr("Default %1").arg(i) );
        }
    }
    else // no n'hi ha de definits al volum, agafem el que ens doni el viewer
    {
        double wl[2];
        m_vectorViewers.value( 0 )->getViewer()->getDefaultWindowLevel( wl );
        m_windowLevelComboBox->insertWindowLevelPreset( wl[0], wl[1], 0, tr("Default") );
    }
    m_windowLevelComboBox->setCurrentIndex( 0 );

    INFO_LOG("Q2DViewerExtension: Donem l'input principal")
    changeViewToAxial();
}

void Q2DViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    m_selectedViewer->changeViewToAxial();
}

void Q2DViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    m_selectedViewer->changeViewToSagital();
}

void Q2DViewerExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    m_selectedViewer->changeViewToCoronal();
}

void Q2DViewerExtension::setView( ViewType view )
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

void Q2DViewerExtension::addColumns( int columns )
{
    QVector<QHBoxLayout*>::Iterator it;
    int posViewer = m_columns;
    Q2DViewerWidget * newViewer;

    while( columns > 0 )
    {
        it = m_qHorizontalLayoutVector.begin();
        m_columns += 1;
        m_totalColumns += 1;
        // Afegim un widget a cada fila per tenir una columna més
        int i = 0;
        while( it != m_qHorizontalLayoutVector.end() )
        {
            newViewer = getNewQ2DViewerWidget();
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

void Q2DViewerExtension::addRows( int rows )
{
    QHBoxLayout *horizontal;
    Q2DViewerWidget *newViewer;
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
            newViewer = getNewQ2DViewerWidget();
            horizontal->addWidget( newViewer );
            m_vectorViewers.push_back( newViewer );
            if( i >= m_columns) newViewer->hide();
        }
        rows--;
    }
}

void Q2DViewerExtension::removeColumns( int columns )
{
    QVector<QHBoxLayout*>::Iterator it = m_qHorizontalLayoutVector.begin();
    int posViewer = m_columns-1;
    Q2DViewerWidget * oldViewer;

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

void Q2DViewerExtension::removeRows( int rows )
{
    int i;
    m_verticalLayout->removeItem(m_verticalLayout->itemAt(m_verticalLayout->count()));
    int posViewer = m_vectorViewers.count()-1;
    Q2DViewerWidget * oldViewer;

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

Q2DViewerWidget* Q2DViewerExtension::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget( m_workingArea );
    newViewer->getViewer()->render();
    (newViewer->getViewer() )->setTool( (m_vectorViewers.value( 0 )->getViewer() )->getCurrentToolName() );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , newViewer->getViewer(), SLOT( setTool(QString) ) );
    connect( newViewer , SIGNAL( selected( Q2DViewerWidget * ) ) , this, SLOT( setViewerSelected( Q2DViewerWidget * ) ) );

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

void Q2DViewerExtension::setGrid( int rows, int columns )
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

void Q2DViewerExtension::setViewerSelected( Q2DViewerWidget * viewer )
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
        connect( m_selectedViewer->getViewer(), SIGNAL( volumeChanged( Volume * ) ), this, SLOT( validePhases() ) );  
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

void Q2DViewerExtension::setVoxelInformationCaptionEnabled(bool option)
{
    ( m_selectedViewer->getViewer() )->setVoxelInformationCaptionEnabled( option );
}

void Q2DViewerExtension::horizontalFlip()
{
    ( m_selectedViewer->getViewer() )->horizontalFlip();
}

void Q2DViewerExtension::verticalFlip()
{
    ( m_selectedViewer->getViewer() )->verticalFlip();
}

void Q2DViewerExtension::setWindowLevel(double wl1 ,double wl2)
{
    ( m_selectedViewer->getViewer() )->setWindowLevel( wl1, wl2 );
}

void Q2DViewerExtension::resetWindowLevelToDefault()
{
    ( m_selectedViewer->getViewer() )->resetWindowLevelToDefault();
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
    m_predefinedSlicesGrid->createPredefinedGrids( m_selectedViewer->getViewer()->getNumberOfSlices() );
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
                connect( volume, SIGNAL(progress(int)), this, SLOT( updateVolumeLoadProgressNotification(int) ) );
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
            if ( m_selectedViewer == viewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
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
            if ( m_selectedViewer == viewer ) setViewerSelected( m_vectorViewers.value( 0 ) );
        }
        columns--;
    }
}

void Q2DViewerExtension::showInformation( int state )
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

}
