#include "slicingwheeltool.h"
#include "q2dviewer.h"
#include "statswatcher.h"
#include "toolproxy.h"

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingWheelTool::SlicingWheelTool(QViewer *viewer, QObject *parent)
: SlicingTool(viewer, parent)
{
    m_toolName = "SlicingWheelTool";
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_2DViewer);
    //m_slicingParent = qobject_cast<SlicingTool*>(parent);
}

SlicingWheelTool::~SlicingWheelTool()
{
    // Estadístiques
    if (!m_wheelSteps.isEmpty())
    {
        StatsWatcher::log("Slicing Wheel Tool: Wheel Record: " + m_wheelSteps);
        m_wheelSteps.clear();
    }
}

void SlicingWheelTool::handleEvent(unsigned long eventID)
{
    if (!m_2DViewer->getInput())
    {
        return;
    }
    
    m_slicingTool = qobject_cast<SlicingTool*>(m_2DViewer->getToolProxy()->getTool("SlicingTool"));
    if(m_slicingTool)
    {
        m_slicingMode = m_slicingTool->getSlicingMode();
    }

    switch (eventID)
    {
        case vtkCommand::MouseWheelForwardEvent:
            m_mouseMovement = false;
            m_viewer->setCursor(QCursor(QPixmap(":/images/slicing.png")));
            SlicingTool::updateIncrement(1);
            m_viewer->unsetCursor();
            // Estadístiques
            m_wheelSteps += QString::number(1) + " ";
            break;

        case vtkCommand::MouseWheelBackwardEvent:
            m_mouseMovement = false;
            m_viewer->setCursor(QCursor(QPixmap(":/images/slicing.png")));
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

}
