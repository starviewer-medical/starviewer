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

#include "windowleveltool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "voilutpresetstooldata.h"
#include "volumehelper.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

namespace udg {

WindowLevelTool::WindowLevelTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolName = "WindowLevelTool";
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);

    m_2DViewer = dynamic_cast<Q2DViewer*>(m_viewer);

    reset();

    connect(m_viewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
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
            if (m_state != None)
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

void WindowLevelTool::reset()
{
    m_state = None;
}

void WindowLevelTool::startWindowLevel()
{
    VoiLut voiLut;

    if (m_2DViewer && m_2DViewer->getNumberOfInputs() > 1 && m_viewer->getInteractor()->GetShiftKey())
    {
        m_state = Burning;
        voiLut = m_2DViewer->getCurrentVoiLutInVolume(1);
    }
    else
    {
        m_state = WindowLevelling;
        voiLut = m_viewer->getCurrentVoiLut();
    }

    updateWindowLevellingBehaviour();

    m_initialWindow = voiLut.getWindowLevel().getWidth();
    m_initialLevel = voiLut.getWindowLevel().getCenter();
    m_initialLut = voiLut.getLut();
    m_initialLut.setName(voiLut.getOriginalLutExplanation());
    m_windowLevelStartPosition = m_viewer->getEventPosition();
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void WindowLevelTool::doWindowLevel()
{
    m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/contrast.svg")));
    m_windowLevelCurrentPosition = m_viewer->getEventPosition();

    QSize size = m_viewer->getRenderWindowSize();

    // Compute normalized delta
    double dx = 4.0 * (m_windowLevelCurrentPosition.x() - m_windowLevelStartPosition.x()) / size.width();
    double dy = 4.0 * (m_windowLevelStartPosition.y() - m_windowLevelCurrentPosition.y()) / size.height();

    // Scale by current values
    if (fabs(m_initialWindow) > 0.01)
    {
        dx = dx * m_initialWindow;
    }
    else
    {
        dx = dx * (m_initialWindow < 0 ? -0.01 : 0.01);
    }
    if (fabs(m_initialLevel) > 0.01)
    {
        dy = dy * m_initialLevel;
    }
    else
    {
        dy = dy * (m_initialLevel < 0 ? -0.01 : 0.01);
    }

    // Abs so that direction does not flip
    if (m_initialWindow < 0.0)
    {
        dx = -1 * dx;
    }
    if (m_initialLevel < 0.0)
    {
        dy = -1 * dy;
    }

    // Compute new window level
    double newWindow;
    double newLevel;
    computeWindowLevelValues(dx, dy, newWindow, newLevel);

    VoiLut voiLut;

    if (m_viewer->getCurrentVoiLut().isWindowLevel())
    {
        voiLut = WindowLevel(newWindow, newLevel);
    }
    else
    {
        double oldX1 = m_initialLut.keys().first();
        double oldX2 = m_initialLut.keys().last();
        double newX1 = newLevel - newWindow / 2.0;
        double newX2 = newLevel + newWindow / 2.0;
        voiLut = VoiLut(m_initialLut.toNewRange(oldX1, oldX2, newX1, newX2), m_initialLut.name());
    }

    if (m_state == WindowLevelling)
    {
        m_viewer->setVoiLut(voiLut);
    }
    else if (m_state == Burning)
    {
        m_2DViewer->setVoiLutInVolume(1, voiLut);
    }
}

void WindowLevelTool::endWindowLevel()
{
    m_viewer->unsetCursor();
    m_state = None;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());
    // Necessari perquè es torni a renderitzar a alta resolució en el 3D
    m_viewer->render();
}

void WindowLevelTool::updateWindowLevellingBehaviour()
{
    m_windowLevellingBehaviour = Default;
    
    if (!m_viewer)
    {
        return;
    }

    if (!m_viewer->hasInput())
    {
        return;
    }

    if (VolumeHelper::isPrimaryPET(m_viewer->getMainInput()) || VolumeHelper::isPrimaryNM(m_viewer->getMainInput()) || m_state == Burning)
    {
        m_windowLevellingBehaviour = FixedMinimum;
    }
}

void WindowLevelTool::computeWindowLevelValues(double deltaX, double deltaY, double &window, double &level)
{
    switch (m_windowLevellingBehaviour)
    {
        case FixedMinimum:
            computeWindowLevelValuesWithFixedMinimumBehaviour(deltaX, window, level);
            break;

        case Default:
        default:
            computeWindowLevelValuesWithDefaultBehaviour(deltaX, deltaY, window, level);
            break;
    }
}

void WindowLevelTool::computeWindowLevelValuesWithFixedMinimumBehaviour(double deltaX, double &window, double &level)
{
    // HACK We use absolute window value to properly handle the windowlevelling when 
    // values have been inverted with the invert tool (window value is negative)
    window = deltaX + fabs(m_initialWindow);
    level = window * 0.5;

    avoidZeroAndNegative(window, level);
    
    // HACK We use this little hack to properly handle the windowlevelling when 
    // values have been inverted with the invert tool (window value is negative)
    if (m_initialWindow < 0)
    {
        window = -window;
    }
}

void WindowLevelTool::computeWindowLevelValuesWithDefaultBehaviour(double deltaX, double deltaY, double &window, double &level)
{
    window = deltaX + m_initialWindow;
    level = m_initialLevel - deltaY;

    avoidZero(window, level);
}

void WindowLevelTool::avoidZero(double &window, double &level)
{
    // Stay away from zero and really
    if (fabs(window) < 0.01)
    {
        window = 0.01 * (window < 0 ? -1 : 1);
    }
    if (fabs(level) < 0.01)
    {
        level = 0.01 * (level < 0 ? -1 : 1);
    }
}

void WindowLevelTool::avoidZeroAndNegative(double &window, double &level)
{
    if (window < 0.01)
    {
        window =  1;
    }
    if (level < 0.01)
    {
        level = 1;
    }
}

}
