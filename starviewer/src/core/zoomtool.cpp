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
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>

namespace udg {

ZoomTool::ZoomTool( Q2DViewer *viewer, QObject *parent )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

ZoomTool::ZoomTool( Q3DViewer *viewer, QObject *parent )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

ZoomTool::ZoomTool( Q3DMPRViewer *viewer, QObject *parent )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
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
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        this->zoom( pow(1.1, 2.0) );
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        // \TODO es podria afegir una variable membre 'm_factor' per poder controlar desde fora com de ràpid és l'augment o disminució del zoom
        this->zoom( pow(1.1, -2.0) );
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

void ZoomTool::zoom( double factor )
{
    if( m_interactorStyle )
    {
        vtkRenderer *renderer = m_interactorStyle->GetCurrentRenderer();
        if( renderer )
        {
            m_interactorStyle->StartDolly();
            // codi extret de void vtkInteractorStyleTrackballCamera::Dolly(double factor)
            vtkCamera *camera = renderer->GetActiveCamera();
            if ( camera->GetParallelProjection() )
            {
                camera->SetParallelScale(camera->GetParallelScale() / factor );
            }
            else
            {
                camera->Dolly(factor);
                if ( m_interactorStyle->GetAutoAdjustCameraClippingRange() )
                {
                    renderer->ResetCameraClippingRange();
                }
            }

            if ( m_interactorStyle->GetInteractor()->GetLightFollowCamera() )
            {
                renderer->UpdateLightsGeometryToFollowCamera();
            }
            m_interactorStyle->GetInteractor()->Render();

            m_interactorStyle->EndDolly();
        }
        else
            DEBUG_LOG( "::zoom(double factor): El renderer és NUL!" );
    }
    else
        DEBUG_LOG( "::zoom(double factor): L'interactor Style és buit!" );
}

}
