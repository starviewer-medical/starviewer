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
#include "logging.h"
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyle.h>
#include <vtkCommand.h>

namespace udg {

ZoomTool::ZoomTool( Q2DViewer *viewer , QObject *parent, const char *name )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

ZoomTool::ZoomTool( Q3DViewer *viewer , QObject *parent, const char *name )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
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
        if( m_interactorStyle )
            m_interactorStyle->Zoom();
        else
            DEBUG_LOG( "::MouseWheelBackwardZoom(): L'interactor Style és buit!" );
    break;

    default:
    break;
    }
}

void ZoomTool::startZoom()
{
    if( m_interactorStyle )
    {
        m_state = ZOOMING;
        m_interactorStyle->StartDolly();
    }
    else
        DEBUG_LOG( "::startZoom(): L'interactor Style és buit!" );
}

void ZoomTool::doZoom()
{
    if( m_interactorStyle )
    {
        if( m_state == ZOOMING )
            m_interactorStyle->Dolly();
    }
    else
        DEBUG_LOG( "::doZoom(): L'interactor Style és buit!" );
}

void ZoomTool::endZoom()
{
    if( m_interactorStyle )
    {
        m_state = NONE;
        m_interactorStyle->EndDolly();
    }
    else
        DEBUG_LOG( "::endZoom(): L'interactor Style és buit!" );
}

}
