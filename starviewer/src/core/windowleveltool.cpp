/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "windowleveltool.h"
#include "q2dviewer.h"
#include <vtkInteractorStyleImage.h> // per controlar el window level
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

WindowLevelTool::WindowLevelTool( Q2DViewer *viewer , QObject *parent, const char *name )
//  : Tool( parent , name )
{
    m_state = NONE;
    m_2DViewer = viewer;
}

WindowLevelTool::~WindowLevelTool()
{
}

void WindowLevelTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startWindowLevel();
    break;

    case vtkCommand::MouseMoveEvent:
        if( m_state == WINDOWLEVELING )
            this->doWindowLevel();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endWindowLevel();
    break;
    
    default:
    break;
    }
}

void WindowLevelTool::startWindowLevel()
{
    m_state = WINDOWLEVELING;
    m_initialWindow = m_2DViewer->getCurrentColorWindow();
    m_initialLevel = m_2DViewer->getCurrentColorLevel();
    m_windowLevelStartPosition[0] = m_2DViewer->getInteractor()->GetEventPosition()[0];
    m_windowLevelStartPosition[1] = m_2DViewer->getInteractor()->GetEventPosition()[1];
}

void WindowLevelTool::doWindowLevel()
{
    m_windowLevelCurrentPosition[0] = m_2DViewer->getInteractor()->GetEventPosition()[0];
    m_windowLevelCurrentPosition[1] = m_2DViewer->getInteractor()->GetEventPosition()[1];
        
    int *size = m_2DViewer->getRenderer()->GetRenderWindow()->GetSize();
    double window = m_initialWindow;
    double level = m_initialLevel;

    // Compute normalized delta
    double dx = 4.0 *( m_windowLevelCurrentPosition[0] - m_windowLevelStartPosition[0]) / size[0];
    double dy = 4.0 *( m_windowLevelStartPosition[1] - m_windowLevelCurrentPosition[1]) / size[1];

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
        dx = -1*dx;
    }
    if (level < 0.0)
    {
        dy = -1*dy;
    }

    // Compute new window level
    double newWindow = dx + window;
    double newLevel;
    newLevel = level - dy;

    // Stay away from zero and really
    if ( fabs(newWindow) < 0.01 )
    {
        newWindow = 0.01 * ( newWindow < 0 ? -1 : 1 );
    }
    if ( fabs(newLevel) < 0.01 )
    {
        newLevel = 0.01 * ( newLevel < 0 ? -1 : 1 );
    }

    m_2DViewer->setWindowLevel( newWindow , newLevel );
}

void WindowLevelTool::endWindowLevel()
{
    m_state = NONE;
}

}
