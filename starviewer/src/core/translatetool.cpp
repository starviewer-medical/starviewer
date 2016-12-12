/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "translatetool.h"
#include "qviewer.h"
#include "logging.h"
// Vtk
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkCommand.h>

namespace udg {

TranslateTool::TranslateTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_state = None;
    m_toolName = "TranslateTool";
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);
}

TranslateTool::~TranslateTool()
{
}

void TranslateTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
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
    m_state = Translating;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void TranslateTool::doTranslate()
{
    if (m_state == Translating)
    {
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/pan.svg")));
        this->pan();
    }
}

void TranslateTool::pan()
{
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    m_viewer->getEventWorldCoordinate(newPickPoint);
    m_viewer->getLastEventWorldCoordinate(oldPickPoint);

    // Camera motion is reversed
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];
    m_viewer->pan(motionVector);
}

void TranslateTool::endTranslate()
{
    m_viewer->unsetCursor();
    m_state = None;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());
    m_viewer->render();
}

}
