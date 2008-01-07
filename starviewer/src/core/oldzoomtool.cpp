/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "oldzoomtool.h"
#include "qviewer.h"
#include "logging.h"
// vtk
#include <vtkInteractorStyle.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

OldZoomTool::OldZoomTool( QViewer *viewer, QObject *parent )
{
    m_state = NONE;
    m_viewer = viewer;
}

OldZoomTool::~OldZoomTool()
{
}

void OldZoomTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startZoom();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doZoom();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endZoom();
    break;

    case vtkCommand::MouseWheelForwardEvent:
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        m_viewer->zoom( pow((double)1.1, 2.0) );
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        m_viewer->zoom( pow((double)1.1, -2.0) );
    break;

    default:
    break;
    }
}

void OldZoomTool::startZoom()
{
    if( m_viewer )
    {
        m_state = ZOOMING;
        m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetDesiredUpdateRate() );
    }
    else
        DEBUG_LOG( "::startZoom(): El viewer és nul!" );
}

void OldZoomTool::doZoom()
{
    if( m_viewer )
    {
        if( m_state == ZOOMING )
        {
            vtkInteractorStyle *interactor = m_viewer->getInteractorStyle();

            double *center = interactor->GetCurrentRenderer()->GetCenter();
            int dy = m_viewer->getEventPositionY() - m_viewer->getLastEventPositionY();
            // TODO el 10.0 és un valor constant que podria refinar-se si es volgués (motion factor)
            double dyf = 10.0 * (double)(dy) / (double)(center[1]);
            m_viewer->zoom( pow((double)1.1, dyf) );
        }
    }
    else
        DEBUG_LOG( "::doZoom(): El viewer és nul!" );
}

void OldZoomTool::endZoom()
{
    if( m_viewer )
    {
        m_state = NONE;
        m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetStillUpdateRate() );
        m_viewer->refresh();
    }
    else
        DEBUG_LOG( "::endZoom(): El viewer és nul!" );
}

}
