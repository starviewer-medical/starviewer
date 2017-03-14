/*************************************************************************************
  Copyright (C) 2017 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2017. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "slicingmousetool.h"
#include "q2dviewer.h"
#include "toolproxy.h"

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingMouseTool::SlicingMouseTool(QViewer *viewer, QObject *parent)
: SlicingTool(viewer, parent)
{
    m_toolName = "SlicingMouseTool";
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_2DViewer);
}

SlicingMouseTool::~SlicingMouseTool()
{
}

void SlicingMouseTool::handleEvent(unsigned long eventID)
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    switch (eventID)
    {
        
        case vtkCommand::LeftButtonPressEvent:
            m_mouseMovement = false;
            this->startSlicing();

        case vtkCommand::MouseMoveEvent:
            m_mouseMovement = true;
            this->doSlicing();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            m_mouseMovement = false;
            this->endSlicing();
            break;
            
        case vtkCommand::KeyPressEvent:
            if (m_viewer->getInteractor()->GetControlKey() && m_inputHasPhases)
            {
                m_forcePhaseMode = true;
                computeImagesForScrollMode();
            }
            break;

        case vtkCommand::KeyReleaseEvent:
            m_forcePhaseMode = false;
            computeImagesForScrollMode();
            break;

        default:
            break;
    }
}

}
