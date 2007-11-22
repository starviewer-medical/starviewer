/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3d2dextension.h"
#include "q3dmprviewer.h"
#include "q2dviewer.h"
#include "mathtools.h" // per càlculs d'interseccions
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "toolsactionfactory.h"
#include "volume.h"
#include "series.h"
// qt
#include <QToolButton>
#include <QSplitter>
#include <QSettings>
#include <QAction>
// vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty2D.h>
#include <vtkImageChangeInformation.h> // per portar a l'origen

namespace udg {

QMPR3D2DExtension::QMPR3D2DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    createActions();
    createConnections();

    m_axialViewEnabledButton->setChecked( true );
    m_sagitalViewEnabledButton->setChecked( true );
    m_coronalViewEnabledButton->setChecked( true );

    createTools();
    createActors();
    updateActors();

    m_axial2DView->enableAnnotation( Q2DViewer::ScalarBarAnnotation, false );
    m_sagital2DView->enableAnnotation( Q2DViewer::ScalarBarAnnotation, false );
    m_coronal2DView->enableAnnotation( Q2DViewer::ScalarBarAnnotation, false );
    m_mpr3DView->orientationMarkerOff();
    m_axial2DView->disableContextMenu();
    m_sagital2DView->disableContextMenu();
    m_coronal2DView->disableContextMenu();
    readSettings();
}

QMPR3D2DExtension::~QMPR3D2DExtension()
{
    writeSettings();

    m_sagitalOverAxialIntersectionAxis->Delete();
    m_coronalOverAxialIntersectionAxis->Delete();
    m_axialOverSagitalIntersectionAxis->Delete();
    m_coronalOverSagitalIntersectionAxis->Delete();
}

void QMPR3D2DExtension::setInput( Volume *input )
{
    m_volume = input;
    m_mpr3DView->setInput( m_volume );
    updateExtension( m_volume );
    m_axial2DView->setInput( m_mpr3DView->getAxialResliceOutput() );
    m_axial2DView->render();
    // posem bé la càmara
    vtkCamera *axialCam = m_axial2DView->getRenderer() ? m_axial2DView->getRenderer()->GetActiveCamera() : NULL;
    if ( axialCam )
    {
        axialCam->SetViewUp(0,-1,0);
        axialCam->SetFocalPoint(0,0,0);
        axialCam->SetPosition(0,0,-1);
        m_axial2DView->getRenderer()->ResetCamera();
    }
    double wl[2];
    m_axial2DView->getDefaultWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );

    m_sagital2DView->setInput( m_mpr3DView->getSagitalResliceOutput() );
    m_sagital2DView->render();

    m_coronal2DView->setInput( m_mpr3DView->getCoronalResliceOutput() );
    m_coronal2DView->render();
    // posem bé la càmara
    vtkCamera *coronalCam = m_coronal2DView->getRenderer() ? m_coronal2DView->getRenderer()->GetActiveCamera() : NULL;
    if ( coronalCam )
    {
        coronalCam->SetViewUp(1,0,0);
        coronalCam->SetPosition(0,0,-1);
        coronalCam->SetFocalPoint(0,0,0);
        m_coronal2DView->getRenderer()->ResetCamera();
    }
    // posem la vista a coronal
    m_mpr3DView->resetViewToCoronal();
    updateActors();
}

void QMPR3D2DExtension::updateExtension( Volume *volume )
{
    m_volume = volume;
    if( m_volume->getSeries()->getNumberOfPhases() > 1 )
        m_phasesAlertLabel->setVisible(true);
    else
        m_phasesAlertLabel->setVisible(false);
}

void QMPR3D2DExtension::createActions()
{
    m_leftRightLayoutAction = new QAction( 0 );
    m_leftRightLayoutAction->setText( tr("Switch horizontal layout") );
    m_leftRightLayoutAction->setStatusTip( tr("Switch horizontal layout") );
    m_leftRightLayoutAction->setIcon( QIcon(":/images/view_left_right.png") );
    m_leftRightLayoutToolButton->setDefaultAction( m_leftRightLayoutAction );

    m_viewsLayoutAction = new QAction( 0 );
    m_viewsLayoutAction->setText( tr("Change views layout") );
    m_viewsLayoutAction->setStatusTip( tr("Change views layout") );
    m_viewsLayoutAction->setIcon( QIcon(":/images/view_sidetree.png") );
    m_viewsLayoutToolButton->setDefaultAction( m_viewsLayoutAction );
}

void QMPR3D2DExtension::createTools()
{
    m_mpr3DView->enableTools();
    m_axial2DView->enableTools();
    m_sagital2DView->enableTools();
    m_coronal2DView->enableTools();

    // Pseudo-tool \TODO ara mateix no ho integrem dins del framework de tools, però potser que més endavant sí
    m_voxelInformationAction = new QAction( 0 );
    m_voxelInformationAction->setText( tr("Voxel Information") );
    m_voxelInformationAction->setShortcut( tr("Ctrl+I") );
    m_voxelInformationAction->setStatusTip( tr("Enable voxel information over cursor") );
    m_voxelInformationAction->setIcon( QIcon(":/images/voxelInformation.png") );
    m_voxelInformationAction->setCheckable( true );
    m_voxelInformationToolButton->setDefaultAction( m_voxelInformationAction );

    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_axial2DView , SLOT( setVoxelInformationCaptionEnabled(bool) ) );
    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_sagital2DView , SLOT( setVoxelInformationCaptionEnabled(bool) ) );
    connect( m_voxelInformationAction , SIGNAL( triggered(bool) ) , m_coronal2DView , SLOT( setVoxelInformationCaptionEnabled(bool) ) );


    m_actionFactory = new ToolsActionFactory( 0 );

    m_windowLevelAction = m_actionFactory->getActionFrom( "WindowLevelTool" );
    m_windowLevelToolButton->setDefaultAction( m_windowLevelAction );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = m_actionFactory->getActionFrom( "TranslateTool" );
    m_translateToolButton->setDefaultAction( m_moveAction );

    m_screenShotAction = m_actionFactory->getActionFrom( "ScreenShotTool" );
    m_screenShotToolButton->setDefaultAction( m_screenShotAction );

    m_rotate3DAction = m_actionFactory->getActionFrom( "3DRotationTool" );
    m_rotate3DToolButton->setDefaultAction( m_rotate3DAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_mpr3DView , SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_axial2DView , SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_sagital2DView , SLOT( setTool(QString) ) );
    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_coronal2DView , SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_screenShotAction );
    m_toolsActionGroup->addAction( m_rotate3DAction );
    // activem la tool de zoom per defecte
    m_windowLevelAction->trigger();
}

void QMPR3D2DExtension::switchBigView()
{
    QWidget *leftWidget, *rightWidget;
    leftWidget = m_horizontalSplitter->widget( 0 );
    rightWidget = m_horizontalSplitter->widget( 1 );

    m_horizontalSplitter->insertWidget( 0 , rightWidget );
    m_horizontalSplitter->insertWidget( 1 , leftWidget );
}

void QMPR3D2DExtension::switchViews()
{
    QWidget *first, *second, *third;
    first = m_verticalSplitter->widget( 0 );
    second = m_verticalSplitter->widget( 1 );
    third = m_verticalSplitter->widget( 2 );

    m_verticalSplitter->insertWidget( 0 , second );
    m_verticalSplitter->insertWidget( 1 , third );
    m_verticalSplitter->insertWidget( 2 , first );
}

void QMPR3D2DExtension::update2DViews()
{
    m_axial2DView->setInput( m_mpr3DView->getAxialResliceOutput() );
    m_axial2DView->getRenderer()->Render();

    m_sagital2DView->setInput( m_mpr3DView->getSagitalResliceOutput() );
    m_sagital2DView->getRenderer()->Render();

    m_coronal2DView->setInput( m_mpr3DView->getCoronalResliceOutput() );
    m_coronal2DView->getRenderer()->Render();

    this->updateActors();

    m_axial2DView->getInteractor()->Render();
    m_sagital2DView->getInteractor()->Render();
    m_coronal2DView->getInteractor()->Render();
}

void QMPR3D2DExtension::createActors()
{
    // Creem les instàncies dels actors
    m_sagitalOverAxialIntersectionAxis = vtkAxisActor2D::New();
    m_coronalOverAxialIntersectionAxis = vtkAxisActor2D::New();
    m_axialOverSagitalIntersectionAxis = vtkAxisActor2D::New();

    m_coronalOverSagitalIntersectionAxis = vtkAxisActor2D::New();

    // Els donem les propietats adequades
    m_sagitalOverAxialIntersectionAxis->AxisVisibilityOn();
    m_sagitalOverAxialIntersectionAxis->TickVisibilityOff();
    m_sagitalOverAxialIntersectionAxis->LabelVisibilityOff();
    m_sagitalOverAxialIntersectionAxis->TitleVisibilityOff();
    m_sagitalOverAxialIntersectionAxis->GetProperty()->SetColor( 1. , .6 , .0 );

    m_coronalOverAxialIntersectionAxis->AxisVisibilityOn();
    m_coronalOverAxialIntersectionAxis->TickVisibilityOff();
    m_coronalOverAxialIntersectionAxis->LabelVisibilityOff();
    m_coronalOverAxialIntersectionAxis->TitleVisibilityOff();
    m_coronalOverAxialIntersectionAxis->GetProperty()->SetColor( .0 , 1. , 1. );

    m_axialOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_axialOverSagitalIntersectionAxis->TickVisibilityOff();
    m_axialOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_axialOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_axialOverSagitalIntersectionAxis->GetProperty()->SetColor( 1. , 1. , .0 );

    m_coronalOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_coronalOverSagitalIntersectionAxis->TickVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->GetProperty()->SetColor( .0 , 1. , 1. );

    // Els afegim als visors corresponents
    m_axial2DView->getRenderer()->AddActor2D( m_sagitalOverAxialIntersectionAxis );
    m_axial2DView->getRenderer()->AddActor2D( m_coronalOverAxialIntersectionAxis );

    m_sagital2DView->getRenderer()->AddActor2D( m_axialOverSagitalIntersectionAxis );
    m_sagital2DView->getRenderer()->AddActor2D( m_coronalOverSagitalIntersectionAxis );
}

void QMPR3D2DExtension::updateActors()
{
    // Passem a sistema de coordenades de món
    m_sagitalOverAxialIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sagitalOverAxialIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_coronalOverAxialIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverAxialIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_axialOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_axialOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_coronalOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

//
//
//
// Projeccions AXIAL
//
//
//
    double r[3] , t[3] , position1[3] , position2[3];

    // projecció sagital sobre axial i viceversa
    MathTools::planeIntersection( m_mpr3DView->getAxialPlaneOrigin() , m_mpr3DView->getAxialPlaneNormal() ,  m_mpr3DView->getSagitalPlaneOrigin() , m_mpr3DView->getSagitalPlaneNormal() , r , t );

    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;

    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;

    m_sagitalOverAxialIntersectionAxis->SetPosition(  position1[0] , position1[1] );
    m_sagitalOverAxialIntersectionAxis->SetPosition2( position2[0] , position2[1] );

    m_axialOverSagitalIntersectionAxis->SetPosition(  position1[1] , position1[2] );
    m_axialOverSagitalIntersectionAxis->SetPosition2( position2[1] , position2[2] );

    // projecció coronal sobre axial
    MathTools::planeIntersection( m_mpr3DView->getAxialPlaneOrigin() , m_mpr3DView->getAxialPlaneNormal() ,  m_mpr3DView->getCoronalPlaneOrigin() , m_mpr3DView->getCoronalPlaneNormal() , r , t );

    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;

    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;

    m_coronalOverAxialIntersectionAxis->SetPosition(  position1[0] , position1[1] );
    m_coronalOverAxialIntersectionAxis->SetPosition2( position2[0] , position2[1] );

//
//
// Projeccions sobre SAGITAL
//
//
    // projecció coronal sobre sagital
    MathTools::planeIntersection( m_mpr3DView->getSagitalPlaneOrigin() , m_mpr3DView->getSagitalPlaneNormal() ,  m_mpr3DView->getCoronalPlaneOrigin() , m_mpr3DView->getCoronalPlaneNormal() , r , t );

    position1[0] = r[0] - t[0]*2000;
    position1[1] = r[1] - t[1]*2000;
    position1[2] = r[2] - t[2]*2000;

    position2[0] = r[0] + t[0]*2000;
    position2[1] = r[1] + t[1]*2000;
    position2[2] = r[2] + t[2]*2000;

    m_coronalOverSagitalIntersectionAxis->SetPosition(  position1[1] , position1[2] );
    m_coronalOverSagitalIntersectionAxis->SetPosition2( position2[1] , position2[2] );
}

void QMPR3D2DExtension::createConnections()
{
    connect( m_axialViewEnabledButton , SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setAxialVisibility(bool)));
    connect( m_sagitalViewEnabledButton , SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setSagitalVisibility(bool)));
    connect( m_coronalViewEnabledButton , SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setCoronalVisibility(bool)));

    connect( m_axialOrientationButton , SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToAxial()));
    connect( m_axialOrientationButton , SIGNAL(clicked()), this , SLOT(update2DViews()));

    connect( m_sagitalOrientationButton , SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToSagital()));
    connect( m_sagitalOrientationButton , SIGNAL(clicked()), this , SLOT(update2DViews()));

    connect( m_coronalOrientationButton , SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToCoronal()));
    connect( m_coronalOrientationButton , SIGNAL(clicked()), this , SLOT(update2DViews()));

    // connexions que determinen els canvis del plans a l'MPR 3D que s'han de reflexar a les vistes 2D
    connect( m_mpr3DView , SIGNAL( planesHasChanged() ) , this , SLOT( update2DViews() ) );
    connect( m_mpr3DView , SIGNAL( planesHasChanged() ) , this , SLOT( updateActors() ) );

    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_mpr3DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_axial2DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_sagital2DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_coronal2DView , SLOT( setWindowLevel(double,double) ) );

    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_mpr3DView , SLOT( resetWindowLevelToDefault() ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_axial2DView , SLOT( resetWindowLevelToDefault() ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_sagital2DView , SLOT( resetWindowLevelToDefault() ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_coronal2DView , SLOT( resetWindowLevelToDefault() ) );

    // sincronitzar window level
    connect( m_axial2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_sagital2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_axial2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_coronal2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_axial2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_mpr3DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_sagital2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_coronal2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_sagital2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_axial2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_sagital2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_mpr3DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_coronal2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_axial2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_coronal2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_sagital2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_coronal2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_mpr3DView , SLOT( setWindowLevel( double , double ) ) );

    // layouts
    connect( m_leftRightLayoutAction , SIGNAL( triggered() ) , this , SLOT( switchBigView() ) );
    connect( m_viewsLayoutAction , SIGNAL( triggered() ) , this , SLOT( switchViews() ) );

    connect( m_mpr3DView, SIGNAL( volumeChanged(Volume *) ), SLOT( updateExtension(Volume *) ) );
}

void QMPR3D2DExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-MPR-3D-2D");

    if( settings.value("horizontalSplitter").toByteArray().isEmpty() )
    {
        QList<int> list;
        list << this->size().width()/2 << this->size().width()/2;
        m_horizontalSplitter->setSizes( list );
    }
    else
        m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );

    if( settings.value("verticalSplitter").toByteArray().isEmpty() )
    {
        QList<int> list;
        list << this->size().height()/3 << this->size().height()/3 << this->size().height()/3;
        m_verticalSplitter->setSizes( list );
    }
    else
        m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );

    settings.endGroup();
}

void QMPR3D2DExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-MPR-3D-2D");

    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );

    settings.endGroup();
}

};  // end namespace udg
