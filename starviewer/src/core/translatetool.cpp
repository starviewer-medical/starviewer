/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "translatetool.h"
#include "qviewer.h"
#include "logging.h"
// vtk
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkCommand.h>

namespace udg {

TranslateTool::TranslateTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent)
{
    m_state = NONE;
    m_toolName = "TranslateTool";
}

TranslateTool::~TranslateTool()
{
}

void TranslateTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::MiddleButtonPressEvent:
        this->startTranslate();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doTranslate();
    break;

    case vtkCommand::MiddleButtonReleaseEvent:
        this->endTranslate();
    break;

    default:
    break;
    }
}

void TranslateTool::startTranslate()
{
    m_state = TRANSLATING;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetDesiredUpdateRate() );
}

void TranslateTool::doTranslate()
{
    if( m_state == TRANSLATING )
    {
        m_viewer->setCursor( QCursor(QPixmap(":/images/move.png")) );
        this->pan();
    }
}

void TranslateTool::pan()
{
    // Codi basat en codi extret de vtkInteractorStyleTrackballActor::Pan()
    // Si fem servir "current renderer" en comptes de "default" el desplaçament no va del tot bé
    vtkRenderer *renderer = m_viewer->getInteractorStyle()->GetCurrentRenderer();
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
                                (double)m_viewer->getEventPositionX(),
                                (double)m_viewer->getEventPositionY(),
                                focalDepth,
                                newPickPoint);

    // Has to recalc old mouse point since the viewport has moved,
    // so can't move it outside the loop

    QViewer::computeDisplayToWorld( renderer,
                                (double)m_viewer->getLastEventPositionX(),
                                (double)m_viewer->getLastEventPositionY(),
                                focalDepth,
                                oldPickPoint );

    // Camera motion is reversed
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];
    m_viewer->pan( motionVector );
}

void TranslateTool::endTranslate()
{
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = NONE;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetStillUpdateRate() );
    m_viewer->refresh();
}

}
