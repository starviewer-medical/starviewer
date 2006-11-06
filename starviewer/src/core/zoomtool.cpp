/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "zoomtool.h"
#include "q2dviewer.h"
#include "q3dviewer.h"
#include "q3dmprviewer.h"
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyle.h>
#include <vtkCommand.h>

namespace udg {

ZoomTool::ZoomTool( Q2DViewer *viewer , QObject *parent, const char *name )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
}

ZoomTool::ZoomTool( Q3DViewer *viewer , QObject *parent, const char *name )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
}

ZoomTool::ZoomTool( Q3DMPRViewer *viewer , QObject *parent, const char *name )
{
    m_state = NONE;
    // \TODO implement me
//     m_interactorStyle = viewer->getInteractorStyle();
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
//         m_interactorStyle->StartDolly();
//         m_interactorStyle->Dolly( pow(1.1, 2.0) );
//         m_interactorStyle->EndDolly();
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        m_interactorStyle->Zoom();
    break;

    default:
    break;
    }
}

void ZoomTool::startZoom()
{
    m_state = ZOOMING;
    m_interactorStyle->StartDolly();
}

void ZoomTool::doZoom()
{
    if( m_state == ZOOMING )
        m_interactorStyle->Dolly();
}

void ZoomTool::endZoom()
{
    m_state = NONE;
    m_interactorStyle->EndDolly();
}

}
