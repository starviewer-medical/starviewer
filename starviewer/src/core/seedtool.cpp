/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seedtool.h"
#include "q2dviewer.h"
#include "volume.h"

#include <QAction>

#include <vtkCommand.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SeedTool::SeedTool( QViewer *viewer, QObject *parent ) : Tool( viewer, parent )
{
    m_state = NONE;
    m_toolName = "SeedTool";
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
    m_seedSlice = -1;
    m_pointActor = vtkActor::New();
    m_point = vtkSphereSource::New();
    m_point->SetRadius( 2 );
    m_pointActor -> GetProperty()->SetColor( 0.85, 0.13, 0.26 );
    
    m_pointMapper = vtkPolyDataMapper::New();
    m_pointMapper->SetInput( m_point->GetOutput() );
    m_pointActor->SetMapper( m_pointMapper );
    
    m_pointActor->VisibilityOff();
    m_2DViewer->getRenderer()-> AddActor( m_pointActor );
    
    connect( m_2DViewer , SIGNAL( sliceChanged(int) ) , this , SLOT( sliceChanged(int) ) );
}


SeedTool::~SeedTool()
{
    m_pointActor-> Delete();
    m_point->Delete();
    m_pointMapper->Delete();
}

void SeedTool::createAction()
{
}

void SeedTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        setSeed();
    break;

    case vtkCommand::MouseMoveEvent:
        doSeeding();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        endSeeding();
    break;

    default:
    break;
    }
}

void SeedTool::setSeed()
{
    m_state=SEEDING;

    m_2DViewer->getCurrentCursorPosition( m_seedPosition );
    
    //es calcula correctament el valor de profunditat per a corretgir el bug #245
    int slice = m_2DViewer->getCurrentSlice();
    double *spacing = m_2DViewer->getInput()->getSpacing();
    double *origin = m_2DViewer->getInput()->getOrigin();
    
        switch( m_2DViewer->getView() )
        {
            case Q2DViewer::Axial:
                m_seedPosition[2] = origin[2] + (slice * spacing[2]);
            break;
            case Q2DViewer::Sagittal:
                m_seedPosition[0] = origin[0] + (slice * spacing[0]);
            break;
            case Q2DViewer::Coronal:
                m_seedPosition[1] = origin[1] + (slice * spacing[1]);
            break;
        }
    //
    
    m_2DViewer->setSeedPosition(m_seedPosition);
    m_point-> SetCenter(m_seedPosition);
    m_seedSlice = m_2DViewer->getCurrentSlice();
    m_pointActor->VisibilityOn();

    m_2DViewer->refresh();
}

void SeedTool::doSeeding( )
{
    if(m_state==SEEDING)
    {
        QString aux;
        m_2DViewer->getCurrentCursorPosition(m_seedPosition);

        m_point-> SetCenter(m_seedPosition);
        m_seedSlice = m_2DViewer->getCurrentSlice();
        m_pointActor->VisibilityOn();
        
        m_2DViewer->refresh();
    }
}

void SeedTool::endSeeding( )
{
    m_state = NONE;
}

void SeedTool::sliceChanged( int s )
{
    if(m_seedSlice==s)
    {
        m_pointActor->VisibilityOn();
    }
    else
    {
        m_pointActor->VisibilityOff();
    }
    m_2DViewer->refresh();
}

}


