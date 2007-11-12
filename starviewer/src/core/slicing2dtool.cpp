/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "slicing2dtool.h"
#include <QAction>
#include <QVTKWidget.h>
#include <vtkRenderWindowInteractor.h>

#include <QTime>
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include <vtkCommand.h>

namespace udg {

Slicing2DTool::Slicing2DTool( Q2DViewer *viewer, QObject *parent )
 : m_slicingMode(SliceMode)
{
    m_state = NONE;
    m_startPosition[0] = 0;
    m_startPosition[1] = 0;
    m_currentPosition[0] = 0;
    m_currentPosition[1] = 0;
    m_time = new QTime();
    m_latestTime = 0;
    m_2DViewer = viewer;
    if( !m_2DViewer )
        DEBUG_LOG( "El 2DViewer és nul!" );
}

Slicing2DTool::~Slicing2DTool()
{
}

void Slicing2DTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startSlicing();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doSlicing();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endSlicing();
    break;

    case vtkCommand::MouseWheelForwardEvent:
        this->updateIncrement( 1 );
        // TODO si tenim l'eina VoxelInformationTool activada cal refrescar les dades
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        this->updateIncrement( -1 );
        // TODO si tenim l'eina VoxelInformationTool activada cal refrescar les dades
    break;

    case vtkCommand::MiddleButtonPressEvent:
        switchSlicingMode();
    break;

    default:
    break;
    }
}

void Slicing2DTool::startSlicing()
{
    if( m_2DViewer )
    {
        m_state = SLICING;
        m_startPosition[0] = m_2DViewer->getInteractor()->GetEventPosition()[0];
        m_startPosition[1] = m_2DViewer->getInteractor()->GetEventPosition()[1];
        m_time->start();
    }
    else
        DEBUG_LOG( "::startSlicing(): El 2DViewer és NUL!" );
}

void Slicing2DTool::doSlicing()
{
    if( m_2DViewer )
    {
        if( m_state == SLICING )
        {
            m_currentPosition[1] = m_2DViewer->getInteractor()->GetEventPosition()[1];
            int dy = m_currentPosition[1] - m_startPosition[1];

            double timeElapsed = ( m_time->elapsed() - m_latestTime )/1000.0; // Es passa a segons
            double acceleracio = (dy*( 1/2300.0 ) )/( timeElapsed*timeElapsed );// 1m = 2300 px aprox.
            m_latestTime = m_time->elapsed();
            m_startPosition[1] = m_currentPosition[1];
            int value = 0;
            if( dy )
            {
                /*value = dy/abs(dy);*/
                /// Canviem un nombre de llesques segons una acceleracio
                value = (int)round(acceleracio);
                if( value == 0 )
                {
                    if( dy >= 0 ) value = 1;
                    else value = -1;
                }
            }
            this->updateIncrement( value );
            DEBUG_LOG( tr("INCREMENT: %1").arg( value ) );
        }
    }
    else
        DEBUG_LOG( "::doSlicing(): El 2DViewer és NUL!" );
}

void Slicing2DTool::endSlicing()
{
    if( m_2DViewer )
        m_state = NONE;
    else
        DEBUG_LOG( "::endSlicing(): El 2DViewer és NUL!" );
}

bool Slicing2DTool::currentInputHasPhases()
{
    bool hasPhases = false;

    if( m_2DViewer )
    {
        if( m_2DViewer->getInput() )
        {
            if( m_2DViewer->getInput()->getNumberOfPhases() > 1 )
                hasPhases = true;
        }
        else
        {
            DEBUG_LOG("L'input del viewer és NULL!");
        }
    }
    else
    {
        DEBUG_LOG("El viewer és NULL!");
    }

    return hasPhases;
}

void Slicing2DTool::switchSlicingMode()
{
    if( currentInputHasPhases() )
    {
        if( m_slicingMode == SliceMode )
            m_slicingMode = PhaseMode;
        else
            m_slicingMode = SliceMode;
    }
}

void Slicing2DTool::updateIncrement(int increment)
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
