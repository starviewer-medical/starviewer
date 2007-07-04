/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "translatetool.h"
#include "q2dviewer.h"
#include "q3dviewer.h"
#include "q3dmprviewer.h"
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyle.h>
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "logging.h"

namespace udg {

TranslateTool::TranslateTool( Q2DViewer *viewer )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

TranslateTool::TranslateTool( Q3DViewer *viewer )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

TranslateTool::TranslateTool( Q3DMPRViewer *viewer )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

TranslateTool::~TranslateTool()
{
}

void TranslateTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startTranslate();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doTranslate();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endTranslate();
    break;

    default:
    break;
    }
}

void TranslateTool::startTranslate()
{
    if( m_interactorStyle )
    {
        m_state = TRANSLATING;
        m_interactorStyle->StartPan();
    }
    else
        DEBUG_LOG( "::startTranslate(): L'interactor Style és buit!" );
}

void TranslateTool::doTranslate()
{
    if( m_interactorStyle )
    {
        if( m_state == TRANSLATING )
            this->pan();
    }
    else
        DEBUG_LOG( "::doTranslate(): L'interactor Style és buit!" );

}

void TranslateTool::pan()
{
    // Codi extret de vtkInteractorStyleTrackballActor::Pan()
    // Si fem servir "current renderer" en comptes de "default" el desplaçament no va del tot bé
    vtkRenderer *renderer = m_interactorStyle->GetDefaultRenderer();
    if( !renderer )
        return;

    double viewFocus[4], focalDepth, viewPoint[3];
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    // Calculate the focal depth since we'll be using it a lot

    vtkCamera *camera = renderer->GetActiveCamera();
    camera->GetFocalPoint( viewFocus );
    QViewer::computeWorldToDisplay( renderer, viewFocus[0], viewFocus[1], viewFocus[2],
                                viewFocus);
    focalDepth = viewFocus[2];

    QViewer::computeDisplayToWorld( renderer,
                                (double)m_interactorStyle->GetInteractor()->GetEventPosition()[0],
                                (double)m_interactorStyle->GetInteractor()->GetEventPosition()[1],
                                focalDepth,
                                newPickPoint);

    // Has to recalc old mouse point since the viewport has moved,
    // so can't move it outside the loop

    QViewer::computeDisplayToWorld( renderer,
                                (double)m_interactorStyle->GetInteractor()->GetLastEventPosition()[0],
                                (double)m_interactorStyle->GetInteractor()->GetLastEventPosition()[1],
                                focalDepth,
                                oldPickPoint );

    // Camera motion is reversed

    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    camera->GetFocalPoint( viewFocus );
    camera->GetPosition( viewPoint );
    camera->SetFocalPoint( motionVector[0] + viewFocus[0],
                            motionVector[1] + viewFocus[1],
                            motionVector[2] + viewFocus[2] );

    camera->SetPosition( motionVector[0] + viewPoint[0],
                        motionVector[1] + viewPoint[1],
                        motionVector[2] + viewPoint[2] );

    if( m_interactorStyle->GetInteractor()->GetLightFollowCamera() )
    {
        renderer->UpdateLightsGeometryToFollowCamera();
    }

    m_interactorStyle->GetInteractor()->Render();
}

void TranslateTool::endTranslate()
{
    if( m_interactorStyle )
    {
        m_state = NONE;
        m_interactorStyle->EndPan();
    }
    else
        DEBUG_LOG( "::endTranslate(): L'interactor Style és buit!" );
}

}
