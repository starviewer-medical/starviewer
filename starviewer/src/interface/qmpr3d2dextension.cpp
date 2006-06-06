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
#include <iostream>

// qt
#include <QToolButton>
#include <QSplitter>
#include <QSettings>
#include <QMessageBox>
#include <QAction>
// vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty2D.h>

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

    createActors();
    updateActors();
    
    readSettings();
}

QMPR3D2DExtension::~QMPR3D2DExtension()
{
    writeSettings();
}

void QMPR3D2DExtension::setInput( Volume *input )
{ 
    m_volume = input; 
    INFO_LOG( "QMPR3D2DExtensions:: Ens donen l'input" );
    m_mpr3DView->setInput( m_volume );

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
    m_axial2DView->getWindowLevel( wl );
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
    m_sagitalOverAxialIntersectionAxis->GetProperty()->SetColor( 1 , 0 , 0 );
    
    m_coronalOverAxialIntersectionAxis->AxisVisibilityOn(); 
    m_coronalOverAxialIntersectionAxis->TickVisibilityOff();
    m_coronalOverAxialIntersectionAxis->LabelVisibilityOff();
    m_coronalOverAxialIntersectionAxis->TitleVisibilityOff();
    m_coronalOverAxialIntersectionAxis->GetProperty()->SetColor( 0 , 0 , 1 );
    
    m_axialOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_axialOverSagitalIntersectionAxis->TickVisibilityOff();
    m_axialOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_axialOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_axialOverSagitalIntersectionAxis->GetProperty()->SetColor( 1 , 1 , 0 );
    
    m_coronalOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_coronalOverSagitalIntersectionAxis->TickVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->GetProperty()->SetColor( 0 , 0 , 1 );
    
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
}

void QMPR3D2DExtension::readSettings()
{
    QSettings settings("GGG", "StarViewer-App-MPR-3D-2D");
    settings.beginGroup("StarViewer-App-MPR-3D-2D");
    
    m_horizontalSplitter->restoreState( settings.value("horizontalSplitter").toByteArray() );
    m_verticalSplitter->restoreState( settings.value("verticalSplitter").toByteArray() );

    settings.endGroup();
}

void QMPR3D2DExtension::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-MPR-3D-2D");
    settings.beginGroup("StarViewer-App-MPR-3D-2D");
    
    settings.setValue("horizontalSplitter", m_horizontalSplitter->saveState() );
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState() );
    
    settings.endGroup();
}

};  // end namespace udg 
