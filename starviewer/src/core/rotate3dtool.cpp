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

#include "rotate3dtool.h"
#include "logging.h"
#include "qviewer.h"
// Vtk
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

Rotate3DTool::Rotate3DTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "Rotate3DTool";
    m_state = None;
    m_interactorStyle = vtkInteractorStyle::SafeDownCast(viewer->getInteractor()->GetInteractorStyle());
    if (!m_interactorStyle)
    {
        DEBUG_LOG("L'interactor Style és buit!");
    }
}

Rotate3DTool::~Rotate3DTool()
{
    m_viewer->unsetCursor();
}

void Rotate3DTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::RightButtonPressEvent:
            this->startRotate3D();
            break;

        case vtkCommand::MouseMoveEvent:
            this->doRotate3D();
            break;

        case vtkCommand::RightButtonReleaseEvent:
            this->endRotate3D();
            break;

        default:
            break;
    }
}

void Rotate3DTool::startRotate3D()
{
    Q_ASSERT(m_interactorStyle);

    if (m_viewer->getInteractor()->GetControlKey())
    {
        m_state = Spinning;
        // TODO podria ser que volguéssim posar-li una icona diferent per quan fem SPIN
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/swivel.svg")));
        m_interactorStyle->StartSpin();
    }
    else
    {
        m_state = Rotating;
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/swivel.svg")));
        m_interactorStyle->StartRotate();
    }
}

void Rotate3DTool::doRotate3D()
{
    Q_ASSERT(m_interactorStyle);

    switch (m_state)
    {
        case Rotating:
            m_interactorStyle->Rotate();
            break;

        case Spinning:
            m_interactorStyle->Spin();
            break;
    }
}

void Rotate3DTool::endRotate3D()
{
    Q_ASSERT(m_interactorStyle);

    m_viewer->unsetCursor();
    switch (m_state)
    {
        case Rotating:
            m_interactorStyle->EndRotate();
            break;

        case Spinning:
            m_interactorStyle->EndSpin();
            break;
    }
    m_state = None;
}

}
