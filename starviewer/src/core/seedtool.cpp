/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seedtool.h"

#include "volume.h"
#include <QAction>

#include "q2dviewer.h"
#include <vtkCommand.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SeedTool::SeedTool( Q2DViewer *viewer, QObject *parent )
// : Tool(parent)
{
    m_state = NONE;
    m_2DViewer = viewer;
    m_seedSlice = -1;
    m_pointActor = vtkActor::New();
}


SeedTool::~SeedTool()
{
    m_pointActor-> Delete();
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

void SeedTool::setSeed( )
{
    m_state=SEEDING;

    m_2DViewer->getCurrentCursorPosition(m_seedPosition);
    m_2DViewer->setSeedPosition(m_seedPosition);

    vtkSphereSource *point = vtkSphereSource::New();
    point->SetRadius(2);
    point-> SetCenter(m_seedPosition);

    m_seedSlice = m_2DViewer->getSlice( );

    m_pointActor -> GetProperty()->SetColor(0.85, 0.13, 0.26);
    vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
    pointMapper->SetInput( point->GetOutput() );
    m_pointActor->SetMapper( pointMapper );
    m_pointActor->VisibilityOn();

    m_2DViewer->getRenderer()-> AddActor( m_pointActor );
    m_2DViewer->getInteractor()->Render();

    connect( m_2DViewer , SIGNAL( sliceChanged(int) ) , this , SLOT( sliceChanged(int) ) );

    pointMapper -> Delete();
    point       -> Delete();

}

void SeedTool::doSeeding( )
{
    if(m_state==SEEDING)
    {
        QString aux;
        m_2DViewer->getCurrentCursorPosition(m_seedPosition);

        vtkSphereSource *point = vtkSphereSource::New();
        point->SetRadius(2);
        point-> SetCenter(m_seedPosition);

        m_seedSlice = m_2DViewer->getSlice( );

        m_pointActor -> GetProperty()->SetColor(0.85, 0.13, 0.26);
        vtkPolyDataMapper *pointMapper = vtkPolyDataMapper::New();
        pointMapper->SetInput( point->GetOutput() );
        m_pointActor->SetMapper( pointMapper );
        m_pointActor->VisibilityOn();

        m_2DViewer->getRenderer()-> AddActor( m_pointActor );
        m_2DViewer->getInteractor()->Render();

        pointMapper -> Delete();
        point       -> Delete();
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
        m_2DViewer->getInteractor()->Render();
    }
    else
    {
        m_pointActor->VisibilityOff();
        m_2DViewer->getInteractor()->Render();
    }
}

}


