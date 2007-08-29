/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextension.h"

#include "volume.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "toolsactionfactory.h"
#include "volumesourceinformation.h"
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

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
 : QWidget( parent ), m_presentationStateAttacher(0)
{
    setupUi( this );
    m_mainVolume = 0;
    m_secondaryVolume = 0;
    m_keyImageNoteAttacher = NULL;
    m_keyImageNote = NULL;

    readSettings();
    createActions();
    createConnections();
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

    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_2DView , SLOT( setVoxelInformationCaptionEnabled(bool) ) );

    m_rotateClockWiseAction = new QAction( 0 );
    m_rotateClockWiseAction->setText( tr("Rotate Clockwise") );
    m_rotateClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Plus );
    m_rotateClockWiseAction->setStatusTip( tr("Rotate the image in clockwise direction") );
    m_rotateClockWiseAction->setIcon( QIcon(":/images/rotateClockWise.png") );
    m_rotateClockWiseToolButton->setDefaultAction( m_rotateClockWiseAction );

    connect( m_rotateClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateClockWise() ) );

    m_rotateCounterClockWiseAction = new QAction( 0 );
    m_rotateCounterClockWiseAction->setText( tr("Rotate Counter Clockwise") );
    m_rotateCounterClockWiseAction->setShortcut( Qt::CTRL + Qt::Key_Minus );
    m_rotateCounterClockWiseAction->setStatusTip( tr("Rotate the image in counter clockwise direction") );
    m_rotateCounterClockWiseAction->setIcon( QIcon(":/images/rotateCounterClockWise.png") );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_rotateCounterClockWiseAction );

    connect( m_rotateCounterClockWiseAction , SIGNAL( triggered() ) , m_2DView , SLOT( rotateCounterClockWise() ) );

    m_flipHorizontalAction = new QAction(0);
    m_flipHorizontalAction->setText( tr("Flip Horizontal") );
    m_flipHorizontalAction->setStatusTip( tr("Flip the image horizontally") );
    m_flipHorizontalAction->setIcon( QIcon(":/images/flipHorizontal.png") );
    m_flipHorizontalToolButton->setDefaultAction( m_flipHorizontalAction );

    connect( m_flipHorizontalAction , SIGNAL( triggered() ) , m_2DView , SLOT( horizontalFlip() ) );

    m_flipVerticalAction = new QAction(0);
    m_flipVerticalAction->setText( tr("Flip Vertical") );
    m_flipVerticalAction->setStatusTip( tr("Flip the image vertically") );
    m_flipVerticalAction->setIcon( QIcon(":/images/flipVertical.png") );
    m_flipVerticalToolButton->setDefaultAction( m_flipVerticalAction );

    connect( m_flipVerticalAction , SIGNAL( triggered() ) , m_2DView , SLOT( verticalFlip() ) );

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

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_2DView, SLOT( setTool(QString) ) );

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
    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar
    connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_spinBox2_1 , SLOT( setValue(int) ) );
    connect( m_spinBox2_1 , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );

    connect( m_axialViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToCoronal() ) );

    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView , SLOT( setWindowLevel(double,double) ) );

    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView , SLOT( resetWindowLevelToDefault() ) );

    // EXTRA!!!!!\TODO es temporal
    // enable/disable presentation states
    connect( m_presentationStateAction, SIGNAL( toggled(bool) ), this, SLOT( enablePresentationState(bool) ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( m_mainVolume );

    // Omplim el combo amb tants window levels com tingui el volum
    double wl[2];
    int wlCount = m_mainVolume->getVolumeSourceInformation()->getNumberOfWindowLevels();
    if( wlCount )
    {
        for( int i = 0; i < wlCount; i++ )
        {
            m_mainVolume->getVolumeSourceInformation()->getWindowLevel( wl, i );
            QString description = m_mainVolume->getVolumeSourceInformation()->getWindowLevelDescription( i );
            if( !description.isEmpty() )
                m_windowLevelComboBox->insertWindowLevelPreset( wl[0], wl[1], i, description );
            else
                m_windowLevelComboBox->insertWindowLevelPreset( wl[0], wl[1], i, tr("Default %1").arg(i) );
        }
    }
    else // no n'hi ha de definits al volum, agafem el que ens doni el viewer
    {
        m_2DView->getDefaultWindowLevel( wl );
        m_windowLevelComboBox->insertWindowLevelPreset( wl[0], wl[1], 0, tr("Default") );
    }
    m_windowLevelComboBox->setCurrentIndex( 0 );

    INFO_LOG("Q2DViewerExtension: Donem l'input principal")
    changeViewToAxial();
}

void Q2DViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    int secondExtent[6];
    if( m_secondaryVolume )
        m_secondaryVolume->getWholeExtent( secondExtent );
    else
        m_mainVolume->getWholeExtent( secondExtent );

    m_spinBox2_1->setMinimum( extent[4] );
    m_spinBox2_1->setMaximum( extent[5] );
    m_slider2_1->setMaximum( extent[5] );
    m_viewText2_1->setText( tr("XY : Axial") );
    m_2DView->setViewToAxial();
    INFO_LOG("Visor per defecte: Canviem a vista axial (Vista 1)")
    m_2DView->render();
}

void Q2DViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    int secondExtent[6];
    if( m_secondaryVolume )
        m_secondaryVolume->getWholeExtent( secondExtent );
    else
        m_mainVolume->getWholeExtent( secondExtent );

    m_spinBox2_1->setMinimum( extent[0] );
    m_spinBox2_1->setMaximum( extent[1] );
    m_slider2_1->setMaximum( extent[1] );
    m_viewText2_1->setText( tr("YZ : Sagital") );
    m_2DView->setViewToSagittal();
    INFO_LOG("Visor per defecte: Canviem a vista sagital (Vista 1)")
    m_2DView->render();
}

void Q2DViewerExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    int secondExtent[6];
    if( m_secondaryVolume )
        m_secondaryVolume->getWholeExtent( secondExtent );
    else
        m_mainVolume->getWholeExtent( secondExtent );

    m_spinBox2_1->setMinimum( extent[2] );
    m_spinBox2_1->setMaximum( extent[3] );
    m_slider2_1->setMaximum( extent[3] );
    m_viewText2_1->setText( tr("XZ : Coronal") );
    m_2DView->setViewToCoronal();
    INFO_LOG("Visor per defecte: Canviem a vista coronal (Vista 1)")
    m_2DView->render();
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
    m_keyImageNoteAttacher = new Q2DViewerKeyImageNoteAttacher(m_2DView, m_keyImageNote);
    m_keyImageNoteAttacher->setVisibleAdditionalInformation(true);
    m_keyImageNoteAttacher->attach();
}

void Q2DViewerExtension::loadPresentationState(const QString &filename)
{
    // Es carrega l'attacher per el viewer principal només
    if( m_presentationStateAttacher != NULL )
    {
        delete m_presentationStateAttacher;
    }
    m_presentationStateAttacher = new Q2DViewerPresentationStateAttacher( m_2DView, qPrintable(filename) );
    m_presentationStateAction->setEnabled( true );
    m_presentationStateAction->setChecked( true );
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
