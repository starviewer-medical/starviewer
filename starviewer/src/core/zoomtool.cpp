/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "zoomtool.h"
#include "qviewer.h"
#include "logging.h"
// vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

ZoomTool::ZoomTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent)
{
    m_state = NONE;
    m_toolName = "ZoomTool";
    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_viewer );
}

ZoomTool::~ZoomTool()
{
}

void ZoomTool::handleEvent( unsigned long eventID )
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
        m_viewer->setCursor( QCursor( QPixmap(":/images/zoom.png") ) );
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        m_viewer->zoom( pow((double)1.1, 2.0) );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        m_viewer->setCursor( QCursor( QPixmap(":/images/zoom.png") ) );
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        m_viewer->zoom( pow((double)1.1, -2.0) );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    default:
    break;
    }
}

void ZoomTool::startZoom()
{
    m_viewer->setCursor( QCursor( QPixmap(":/images/zoom.png") ) );
    m_state = ZOOMING;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetDesiredUpdateRate() );
}

void ZoomTool::doZoom()
{
    if( m_state == ZOOMING )
    {
        double *center = m_viewer->getRenderer()->GetCenter();
        int dy = m_viewer->getEventPositionY() - m_viewer->getLastEventPositionY();
        // TODO el 10.0 és un valor constant que podria refinar-se si es volgués (motion factor)
        double dyf = 10.0 * (double)(dy) / (double)(center[1]);
        m_viewer->zoom( pow((double)1.1, dyf) );
    }
}

void ZoomTool::endZoom()
{
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = NONE;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetStillUpdateRate() );
}

}
