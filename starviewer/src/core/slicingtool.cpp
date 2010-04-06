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
#include "statswatcher.h"

//vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingTool::SlicingTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_slicingMode(SliceMode), m_mouseMovement(false), m_numberOfImages(1), m_screenSize(0), m_inputHasPhases(false), m_forcePhaseMode(false)
{
    m_state = None;
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
    inputChanged( m_2DViewer->getInput() );
}

SlicingTool::~SlicingTool()
{
    // estadístiques
    if( !m_wheelSteps.isEmpty() )
    {
        StatsWatcher::log( "Slicing Tool: Wheel Record: " + m_wheelSteps );
        m_wheelSteps.clear();
    }
}

void SlicingTool::handleEvent( unsigned long eventID )
{
    if( !m_2DViewer->getInput() )
        return;

    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        m_mouseMovement = false;
        this->startSlicing();
        // estadístiques
        if( !m_wheelSteps.isEmpty() )
        {
            StatsWatcher::log( "Slicing Tool: Wheel Record: " + m_wheelSteps );
            m_wheelSteps.clear();
        }
    break;

    case vtkCommand::MouseMoveEvent:
        m_mouseMovement = true;
        this->doSlicing();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        m_mouseMovement = false;
        this->endSlicing();
        // estadístiques
        if( !m_scrollSteps.isEmpty() )
        {
            StatsWatcher::log( "Slicing Tool: Button Scroll Record: " + m_scrollSteps + " ::Over a total of " + QString::number(m_numberOfImages) + " images" );
            m_scrollSteps.clear();
        }
    break;

    case vtkCommand::MouseWheelForwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( 1 );
        m_viewer->setCursor( Qt::ArrowCursor );
        // estadístiques
        m_wheelSteps += QString::number(1) + " ";
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( -1 );
        m_viewer->setCursor( Qt::ArrowCursor );
        // estadístiques
        m_wheelSteps += QString::number(-1) + " ";
    break;

    case vtkCommand::MiddleButtonPressEvent:
        m_mouseMovement = false;
    break;

    case vtkCommand::MiddleButtonReleaseEvent:
        // TODO aquest comportament de fer switch es podria eliminar ja que no és gaire usable
        // de moment es manté perquè ja tenim un conjunt d'usuaris acostumats a aquest comportament
        if( !m_mouseMovement )
            switchSlicingMode();
    break;

    case vtkCommand::KeyPressEvent:
        if( m_viewer->getInteractor()->GetControlKey() && m_inputHasPhases )
        {
            m_forcePhaseMode = true;
            computeImagesForScrollMode();
            StatsWatcher::log( "FORCE phase mode with Ctrl key" );
        }
        break;
    
    case vtkCommand::KeyReleaseEvent:
        m_forcePhaseMode = false;
        computeImagesForScrollMode();
        StatsWatcher::log( "Disable FORCED phase mode releasing Ctrl key" );
        break;

    default:
    break;
    }
}

void SlicingTool::startSlicing()
{
    m_state = Slicing;
    m_2DViewer->getEventPosition( m_startPosition );
	// calculem el nombre d'imatges que manipulem
    computeImagesForScrollMode();
	m_screenSize = m_2DViewer->getRenderWindowSize();
}

void SlicingTool::doSlicing()
{
    if( m_state == Slicing )
    {
		Q_ASSERT( m_screenSize );
		m_viewer->setCursor( QCursor(QPixmap(":/images/slicing.png")) );
        m_currentPosition[1] = m_2DViewer->getEventPositionY();

        // increment normalitzat segons la mida de la finestra i el nombre de llesques
        double increase = (1.75 * ( m_currentPosition[1] - m_startPosition[1] ) / (double)m_screenSize[1]) * m_numberOfImages;
        m_startPosition[1] = m_currentPosition[1];

        int value = 0;
        // Canviem un nombre de llesques segons el desplaçament del mouse
        value = (int)qRound(increase);
        if( value == 0 )
        {
            if( increase > 0 )
                value = 1;
            else if ( increase < 0 )
                value = -1;
        }
        this->updateIncrement( value );
        // estadístiques
        m_scrollSteps += QString::number(value) + " ";
    }
}

void SlicingTool::endSlicing()
{
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = None;
}

void SlicingTool::inputChanged( Volume *input )
{
    chooseBestDefaultScrollMode(input);
    m_mouseMovement = false;
    m_state = None;
    m_inputHasPhases = false;
    if( input )
    {
        if( input->getNumberOfPhases() > 1 )
            m_inputHasPhases = true;
    }
    else
    {
        DEBUG_LOG("L'input introduit és NULL!");
    }
}

void SlicingTool::switchSlicingMode()
{
    QString statMessage = "SlicingTool: ";
    if( m_inputHasPhases )
    {
        if( m_slicingMode == SliceMode )
        {
            m_slicingMode = PhaseMode;
            statMessage += "Switch from slice mode to phase mode";
        }
        else
        {
            m_slicingMode = SliceMode;
            statMessage += "Switch from phase mode to slice mode";
        }
    }
    else
        statMessage += "Try to switch slicing mode with input with no phases";

    StatsWatcher::log( statMessage );
}

void SlicingTool::updateIncrement(int increment)
{
    // si mantenim control apretat sempe mourem fases independentment de l'slicing mode
    if( m_forcePhaseMode )
        m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + increment );
    else // altrament continuem amb el comportament habitual
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

void SlicingTool::computeImagesForScrollMode()
{
    if( m_forcePhaseMode )
        m_numberOfImages = m_2DViewer->getInput()->getNumberOfPhases();
    else
    {
	    if( m_slicingMode == SliceMode )
		    m_numberOfImages = m_2DViewer->getMaximumSlice();
	    else
		    m_numberOfImages = m_2DViewer->getInput()->getNumberOfPhases();
    }
}

void SlicingTool::chooseBestDefaultScrollMode( Volume *input )
{
    // per defecte sempre serà aquest excepte quan només tenim 1 imatge i tenim fases
    m_slicingMode = SliceMode;
    if( input )
    {
        if( input->getNumberOfPhases() > 1  && m_2DViewer->getMaximumSlice() <= 1 )
        {
            m_slicingMode = PhaseMode;
            StatsWatcher::log("Slicing Tool: Default Scroll Mode = PHASE");
        }
        else
            StatsWatcher::log("Slicing Tool: Default Scroll Mode = SLICE");
    }
}

}

