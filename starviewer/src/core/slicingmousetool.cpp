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
#include "statswatcher.h"
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
    // Estadístiques
    if (!m_wheelSteps.isEmpty())
    {
        StatsWatcher::log("Slicing Wheel Tool: Wheel Record: " + m_wheelSteps);
        m_wheelSteps.clear();
    }
}

void SlicingMouseTool::handleEvent(unsigned long eventID)
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    /// We need to have the same slicing mode as slicing tool to prevent slicing mode incongruences
    updateSlicingModeAccordingToSlicingTool();
    
    switch (eventID)
    {
        case vtkCommand::MouseWheelForwardEvent:
            m_mouseMovement = false;
            m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll.svg")));
            SlicingTool::updateIncrement(1);
            m_viewer->unsetCursor();
            // Estadístiques
            m_wheelSteps += QString::number(1) + " ";
            break;

        case vtkCommand::MouseWheelBackwardEvent:
            m_mouseMovement = false;
            m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll.svg")));
            SlicingTool::updateIncrement(-1);
            m_viewer->unsetCursor();
            // Estadístiques
            m_wheelSteps += QString::number(-1) + " ";
            break;

        //Per tenir constancia de les estadístiques
        case vtkCommand::LeftButtonPressEvent:
            if (!m_wheelSteps.isEmpty())
            {
                StatsWatcher::log("Slicing Wheel Tool: Wheel Record: " + m_wheelSteps);
                m_wheelSteps.clear();
            }
            break;
        
        case vtkCommand::MouseMoveEvent:
            m_mouseMovement = true;
            break;

        case vtkCommand::MiddleButtonPressEvent:
            m_mouseMovement = false;
            break;

        case vtkCommand::MiddleButtonReleaseEvent:
            // TODO aquest comportament de fer switch es podria eliminar ja que no és gaire usable
            // de moment es manté perquè ja tenim un conjunt d'usuaris acostumats a aquest comportament
            if (!m_mouseMovement)
            {
                switchSlicingMode();
            }
            break;

        case vtkCommand::KeyPressEvent:
            if (m_viewer->getInteractor()->GetControlKey() && m_inputHasPhases)
            {
                m_forcePhaseMode = true;
                computeImagesForScrollMode();
                StatsWatcher::log("FORCE phase mode with Ctrl key");
            }
            break;

        case vtkCommand::KeyReleaseEvent:
            m_forcePhaseMode = false;
            computeImagesForScrollMode();
            StatsWatcher::log("Disable FORCED phase mode releasing Ctrl key");
            break;

        default:
            break;
    }
}

void SlicingMouseTool::updateSlicingModeAccordingToSlicingTool()
{
    SlicingTool *slicingTool = qobject_cast<SlicingTool*>(m_2DViewer->getToolProxy()->getTool("SlicingTool"));
    if (slicingTool)
    {
        m_slicingMode = slicingTool->getSlicingMode();
    }
}

}
