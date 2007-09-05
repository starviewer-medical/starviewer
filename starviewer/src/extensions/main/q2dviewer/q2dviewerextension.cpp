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
#include <QAction>
#include <QSettings>
// EXTRA!!! \TODO es temporal
#include <QFileDialog>
#include "keyimagenote.h"
#include "q2dviewerkeyimagenoteattacher.h"
#include "q2dviewerpresentationstateattacher.h"
// VTK
#include <vtkRenderer.h>
#include "slicing2dtool.h"
// Menu
#include "menugridwidget.h"
#include "tablemenu.h"

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
 : QWidget( parent ), m_presentationStateAttacher(0)
{
    setupUi( this );
    m_mainVolume = 0;
    m_keyImageNoteAttacher = NULL;
    m_keyImageNote = NULL;

    m_patient = NULL;
    m_viewer = new Q2DViewerWidget( m_workingArea );
    m_selectedViewer = m_viewer;

    readSettings();
    createActions();
    createConnections();
    initLayouts();
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
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Sagital") );
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
    m_roiAction->setIcon( QIcon(":/images/roi.png") );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_viewer->m_2DView, SLOT( setTool(QString) ) );

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
    connect( m_viewer , SIGNAL( selected( Q2DViewerWidget * ) ) , this, SLOT( setViewerSelected( Q2DViewerWidget * ) ) );

    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar
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

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), this, SLOT( enablePresentationState(bool) ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_viewer->setInput( m_mainVolume );

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
        m_viewer->m_2DView->getDefaultWindowLevel( wl );
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
    m_keyImageNoteAttacher = new Q2DViewerKeyImageNoteAttacher( m_viewer->m_2DView, m_keyImageNote );
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
    m_presentationStateAttacher = new Q2DViewerPresentationStateAttacher( m_viewer->m_2DView, qPrintable(filename) );
    m_presentationStateAction->setEnabled( true );
    m_presentationStateAction->setChecked( true );
}

void Q2DViewerExtension::initLayouts()
{
    m_rows = 1;
    m_columns = 1;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    m_gridLayout->setMargin(0);

    m_verticalLayout = new QVBoxLayout();
    m_verticalLayout->setSpacing(0);
    m_verticalLayout->setMargin(0);

    QHBoxLayout *horizontal = new QHBoxLayout();
    horizontal->setSpacing(0);
    horizontal->setMargin(0);

    horizontal->addWidget( m_viewer );
    m_verticalLayout->addLayout( horizontal,0 );
    m_gridLayout->addLayout( m_verticalLayout,0,0 );

    m_qHorizontalLayoutVector.push_back( horizontal );
    m_vectorViewers.push_back( m_viewer );
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
        // Afegim un widget a cada fila per tenir una columna més
        while( it != m_qHorizontalLayoutVector.end() )
        {
            newViewer = getNewQ2DViewerWidget();
            (*it)->addWidget( newViewer );
            m_vectorViewers.insert( posViewer,newViewer );
            posViewer += m_columns;
            it++;
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
        //Afegim tants widgets com columnes
        for(i = 0; i < m_columns; i++)
        {
            newViewer = getNewQ2DViewerWidget();
            horizontal->addWidget( newViewer );
            m_vectorViewers.push_back( newViewer );
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
            if ( m_selectedViewer == oldViewer ) setViewerSelected( m_viewer );
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
            if ( m_selectedViewer == oldViewer ) setViewerSelected( m_viewer );
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
    newViewer->setInput ( m_mainVolume );
    (newViewer->m_2DView)->setTool( (m_viewer->m_2DView)->getCurrentToolName() );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , newViewer->m_2DView, SLOT( setTool(QString) ) );
    connect( newViewer , SIGNAL( selected( Q2DViewerWidget * ) ) , this, SLOT( setViewerSelected( Q2DViewerWidget * ) ) );

    return newViewer;
}

void Q2DViewerExtension::setGrid( int rows, int columns )
{

    if( m_rows > rows ) removeRows( m_rows - rows);
    else if ( m_rows < rows ) addRows( rows - m_rows );

    if( m_columns > columns ) removeColumns( m_columns - columns );
    else if ( m_columns < columns ) addColumns( columns - m_columns );

}

void Q2DViewerExtension::setViewerSelected( Q2DViewerWidget * viewer )
{
    if ( viewer != m_selectedViewer )
    {

        m_selectedViewer->setSelected( false );
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected( true );

    }
}

void Q2DViewerExtension::rotateClockWise()
{
    ( m_selectedViewer->m_2DView )->rotateClockWise();
}

void Q2DViewerExtension::rotateCounterClockWise()
{
    ( m_selectedViewer->m_2DView )->rotateCounterClockWise();
}

void Q2DViewerExtension::setVoxelInformationCaptionEnabled(bool option)
{
    ( m_selectedViewer->m_2DView )->setVoxelInformationCaptionEnabled( option );
}

void Q2DViewerExtension::horizontalFlip()
{
    ( m_selectedViewer->m_2DView )->horizontalFlip();
}

void Q2DViewerExtension::verticalFlip()
{
    ( m_selectedViewer->m_2DView )->verticalFlip();
}

void Q2DViewerExtension::setWindowLevel(double wl1 ,double wl2)
{
    ( m_selectedViewer->m_2DView )->setWindowLevel( wl1, wl2 );
}

void Q2DViewerExtension::resetWindowLevelToDefault()
{
    ( m_selectedViewer->m_2DView )->resetWindowLevelToDefault();
}

void Q2DViewerExtension::showPredefinedGrid()
{
    int i;
    int numberSeries = 0;

    MenuGridWidget * menuGrid = new MenuGridWidget();
    menuGrid->move( m_buttonGrid->x(),( m_buttonGrid->y() + 95 ) );

    QList<Study *> listStudies = m_patient->getStudies();

    for( i = 0; i < listStudies.size(); i++ )
    {
        numberSeries += listStudies.value( i )->getNumberOfSeries();
    }

    menuGrid->createPredefinedGrids( numberSeries );
    menuGrid->show();

    connect( menuGrid , SIGNAL( selectedGrid( int , int ) ) , this, SLOT( setGrid( int, int ) ) );
}

void Q2DViewerExtension::showInteractiveTable()
{
    TableMenu * tableMenu = new TableMenu();
    tableMenu->move( m_buttonGrid->x(),( m_buttonGrid->y() + 95 ) );
    tableMenu->show();

    connect( tableMenu , SIGNAL( selectedGrid( int , int ) ) , this, SLOT( setGrid( int, int ) ) );
}

void Q2DViewerExtension::showPredefinedImageGrid()
{
    MenuGridWidget * menuGrid = new MenuGridWidget();
    menuGrid->move( m_imageGrid->x(),( m_imageGrid->y() + 95 ) );
    menuGrid->createPredefinedGrids( m_selectedViewer->m_2DView->getNumberOfSlices() );
    menuGrid->show();

    connect( menuGrid , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->m_2DView, SLOT( setGrid( int, int ) ) );
}

void Q2DViewerExtension::showInteractiveImageTable()
{
    TableMenu * tableMenu = new TableMenu();
    tableMenu->move( m_imageGrid->x(),( m_imageGrid->y() + 95 ) );
    tableMenu->show();

    connect( tableMenu , SIGNAL( selectedGrid( int , int ) ) , m_selectedViewer->m_2DView, SLOT( setGrid( int, int ) ) );
}

Patient* Q2DViewerExtension::getPatient() const
{
    return m_patient;
}

void Q2DViewerExtension::setPatient( Patient *patient )
{
    m_patient = patient;
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
