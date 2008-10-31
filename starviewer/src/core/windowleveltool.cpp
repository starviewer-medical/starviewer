/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "windowleveltool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

namespace udg {

WindowLevelTool::WindowLevelTool( QViewer *viewer, QObject *parent )  
: Tool( viewer, parent )
{
    m_state = NONE;
    m_toolName = "WindowLevelTool";
    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_viewer );
}

WindowLevelTool::~WindowLevelTool()
{
}

void WindowLevelTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::RightButtonPressEvent:
        this->startWindowLevel();
    break;

    case vtkCommand::MouseMoveEvent:
        if( m_state == WINDOWLEVELING )
            this->doWindowLevel();
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
    m_state = WINDOWLEVELING;
    double wl[2];
    m_viewer->getCurrentWindowLevel(wl);
    m_initialWindow = wl[0];
    m_initialLevel = wl[1];
    m_viewer->getEventPosition( m_windowLevelStartPosition );
	m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetDesiredUpdateRate() );
}

void WindowLevelTool::doWindowLevel()
{
	m_viewer->setCursor( QCursor(QPixmap(":/images/windowLevel.png")) );
    m_viewer->getEventPosition( m_windowLevelCurrentPosition );

    int *size = m_viewer->getRenderWindowSize();
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
    m_viewer->getWindowLevelData()->setCustomWindowLevel( newWindow , newLevel );
}

void WindowLevelTool::endWindowLevel()
{
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = NONE;
	m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate( m_viewer->getInteractor()->GetStillUpdateRate() );
	m_viewer->refresh(); // necessari perquè es torni a renderitzar a alta resolució en el 3D
}

}
