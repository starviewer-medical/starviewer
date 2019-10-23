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

#include "zoomtool.h"
#include "qviewer.h"
#include "logging.h"
// Vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

ZoomTool::ZoomTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_state = None;
    m_toolName = "ZoomTool";
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);
}

ZoomTool::~ZoomTool()
{
    m_viewer->unsetCursor();
}

void ZoomTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
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

        default:
        break;
    }
}

void ZoomTool::startZoom()
{
    m_state = Zooming;
    m_zoomCenter = m_viewer->getEventPosition();
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void ZoomTool::doZoom()
{
    if (m_state == Zooming)
    {
        constexpr double MotionFactor = 10.0;
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/zoom.svg")));
        int dy = m_viewer->getEventPosition().y() - m_viewer->getLastEventPosition().y();
        double dyf = MotionFactor * dy / (m_viewer->height() / 2);
        m_viewer->zoom(pow(1.1, dyf), m_zoomCenter);
    }
}

void ZoomTool::endZoom()
{
    if (m_state == Zooming)
    {
        m_viewer->unsetCursor();
        m_state = None;
        m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());
        m_viewer->render();
    }
}

}
