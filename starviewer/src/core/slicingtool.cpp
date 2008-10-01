/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "slicingtool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"

//vtk
#include <vtkCommand.h>

namespace udg {

SlicingTool::SlicingTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_slicingMode(SliceMode), m_mouseMovement(false), m_numberOfImages(1), m_screenSize(0)
{
    m_state = NONE;
    m_toolName = "SlicingTool";
    m_startPosition[0] = 0;
    m_startPosition[1] = 0;
    m_currentPosition[0] = 0;
    m_currentPosition[1] = 0;
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_2DViewer );

    // cada cop que canvïi l'input cal fer algunes inicialitzacions
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( inputChanged(Volume *) ) );
}

SlicingTool::~SlicingTool()
{
}

void SlicingTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        m_mouseMovement = false;
        this->startSlicing();
    break;

    case vtkCommand::MouseMoveEvent:
        m_mouseMovement = true;
        this->doSlicing();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        m_mouseMovement = false;
        this->endSlicing();
    break;

    case vtkCommand::MouseWheelForwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( 1 );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( -1 );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    case vtkCommand::MiddleButtonPressEvent:
        m_mouseMovement = false;
    break;

    case vtkCommand::MiddleButtonReleaseEvent:
        if( !m_mouseMovement )
            switchSlicingMode();
    break;

    default:
    break;
    }
}

void SlicingTool::startSlicing()
{
    m_viewer->setCursor( QCursor(QPixmap(":/images/slicing.png")) );
    m_state = SLICING;
    m_2DViewer->getEventPosition( m_startPosition );
	// calculem el nombre d'imatges que manipulem
	if( m_slicingMode == SliceMode )
		m_numberOfImages = m_2DViewer->getMaximumSlice();
	else
		m_numberOfImages = m_2DViewer->getInput()->getNumberOfPhases();

	m_screenSize = m_2DViewer->getRenderWindowSize();
}

void SlicingTool::doSlicing()
{
    if( m_state == SLICING )
    {
		Q_ASSERT( m_screenSize );
        m_currentPosition[1] = m_2DViewer->getEventPositionY();

        // increment normalitzat segons la mida de la finestra i el nombre de llesques
        double increase = (1.75 * ( m_currentPosition[1] - m_startPosition[1] ) / (double)m_screenSize[1]) * m_numberOfImages;
        m_startPosition[1] = m_currentPosition[1];

        int value = 0;
        // Canviem un nombre de llesques segons el desplaçament del mouse
        value = (int)qRound(increase);
        if( value == 0 )
        {
            if( increase >= 0 )
                value = 1;
            else
                value = -1;
        }
        this->updateIncrement( value );
    }
}

void SlicingTool::endSlicing()
{
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = NONE;
}

void SlicingTool::inputChanged( Volume *input )
{
    Q_UNUSED(input);
    m_slicingMode = SliceMode;
    m_mouseMovement = false;
}

bool SlicingTool::currentInputHasPhases()
{
    bool hasPhases = false;

    if( m_2DViewer->getInput() )
    {
        if( m_2DViewer->getInput()->getNumberOfPhases() > 1 )
            hasPhases = true;
    }
    else
    {
        DEBUG_LOG("L'input del viewer és NULL!");
    }

    return hasPhases;
}

void SlicingTool::switchSlicingMode()
{
    if( currentInputHasPhases() )
    {
        if( m_slicingMode == SliceMode )
            m_slicingMode = PhaseMode;
        else
            m_slicingMode = SliceMode;
    }
}

void SlicingTool::updateIncrement(int increment)
{
    switch( m_slicingMode )
    {
        case SliceMode:
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() + increment );
            break;

        case PhaseMode:
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + increment );
            break;
    }
}

}

