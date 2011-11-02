#include "zoomtool.h"
#include "qviewer.h"
#include "logging.h"
#include "q3dviewer.h"
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
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void ZoomTool::doZoom()
{
    if (m_state == Zooming)
    {
        m_viewer->setCursor(QCursor(QPixmap(":/images/zoom.png")));
        double *center = m_viewer->getRenderer()->GetCenter();
        int dy = m_viewer->getEventPositionY() - m_viewer->getLastEventPositionY();
        // TODO el 10.0 és un valor constant que podria refinar-se si es volgués (motion factor)
        double dyf = 10.0 * (double)(dy) / (double)(center[1]);
        m_viewer->zoom(pow((double)1.1, dyf));
    }
}

void ZoomTool::endZoom()
{
    if (m_state == Zooming)
    {
        m_viewer->setCursor(Qt::ArrowCursor);
        m_state = None;
        m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());

        Q3DViewer *q3DViewer = qobject_cast<Q3DViewer*>(m_viewer);
        if (q3DViewer)
        {
            // Necessari perquè es torni a renderitzar a alta resolució en el 3D
            m_viewer->render();
        }
    }
}

}
