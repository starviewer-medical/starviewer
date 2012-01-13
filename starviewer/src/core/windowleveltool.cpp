#include "windowleveltool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

namespace udg {

WindowLevelTool::WindowLevelTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_state = None;
    m_toolName = "WindowLevelTool";
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);
}

WindowLevelTool::~WindowLevelTool()
{
    m_viewer->unsetCursor();
}

void WindowLevelTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::RightButtonPressEvent:
            this->startWindowLevel();
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state == WindowLevelling)
            {
                this->doWindowLevel();
            }
            break;

        case vtkCommand::RightButtonReleaseEvent:
            this->endWindowLevel();
            break;

        default:
            break;
    }
}

void WindowLevelTool::startWindowLevel()
{
    m_state = WindowLevelling;
    double wl[2];
    m_viewer->getCurrentWindowLevel(wl);
    m_initialWindow = wl[0];
    m_initialLevel = wl[1];
    m_windowLevelStartPosition = m_viewer->getEventPosition();
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void WindowLevelTool::doWindowLevel()
{
    m_viewer->setCursor(QCursor(QPixmap(":/images/windowLevel.png")));
    m_windowLevelCurrentPosition = m_viewer->getEventPosition();

    QSize size = m_viewer->getRenderWindowSize();
    double window = m_initialWindow;
    double level = m_initialLevel;

    // Compute normalized delta
    double dx = 4.0 * (m_windowLevelCurrentPosition.x() - m_windowLevelStartPosition.x()) / size.width();
    double dy = 4.0 * (m_windowLevelStartPosition.y() - m_windowLevelCurrentPosition.y()) / size.height();

    // Scale by current values
    if (fabs(window) > 0.01)
    {
        dx = dx * window;
    }
    else
    {
        dx = dx * (window < 0 ? -0.01 : 0.01);
    }
    if (fabs(level) > 0.01)
    {
        dy = dy * level;
    }
    else
    {
        dy = dy * (level < 0 ? -0.01 : 0.01);
    }

    // Abs so that direction does not flip
    if (window < 0.0)
    {
        dx = -1 * dx;
    }
    if (level < 0.0)
    {
        dy = -1 * dy;
    }

    // Compute new window level
    double newWindow = dx + window;
    double newLevel;
    newLevel = level - dy;

    // Stay away from zero and really
    if (fabs(newWindow) < 0.01)
    {
        newWindow = 0.01 * (newWindow < 0 ? -1 : 1);
    }
    if (fabs(newLevel) < 0.01)
    {
        newLevel = 0.01 * (newLevel < 0 ? -1 : 1);
    }
    m_viewer->getWindowLevelData()->setCustomWindowLevel(newWindow, newLevel);
}

void WindowLevelTool::endWindowLevel()
{
    m_viewer->setCursor(Qt::ArrowCursor);
    m_state = None;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());
    // Necessari perquè es torni a renderitzar a alta resolució en el 3D
    m_viewer->render();
}

}
