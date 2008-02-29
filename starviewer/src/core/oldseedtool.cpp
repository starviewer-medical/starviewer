/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "oldseedtool.h"

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

OldSeedTool::OldSeedTool( Q2DViewer *viewer, QObject *parent )
// : Tool(parent)
{
    m_state = NONE;
    m_2DViewer = viewer;
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


OldSeedTool::~OldSeedTool()
{
    m_pointActor-> Delete();
    m_point->Delete();
    m_pointMapper->Delete();
}

void OldSeedTool::createAction()
{
}

void OldSeedTool::handleEvent( unsigned long eventID )
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

void OldSeedTool::setSeed()
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
            case Q2DViewer::Sagital:
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

void OldSeedTool::doSeeding( )
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

void OldSeedTool::endSeeding( )
{
    m_state = NONE;
}

void OldSeedTool::sliceChanged( int s )
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


