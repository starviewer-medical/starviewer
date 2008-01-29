/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "slicingkeyboardtool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
//qt
#include <QTime>
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingKeyboardTool::SlicingKeyboardTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent)
{
    m_toolName = "SlicingKeyboardTool";
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
}

void SlicingKeyboardTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::KeyPressEvent:
    {
        QString keySymbol = m_2DViewer->getInteractor()->GetKeySym();
        if( keySymbol == "Home" )
        {
            m_2DViewer->setSlice(0);
        }
        else if( keySymbol == "End" )
        {
            m_2DViewer->setSlice( m_2DViewer->getNumberOfSlices() );
        }
        else if( keySymbol == "Up" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() + 1 );
        }
        else if( keySymbol == "Down" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() - 1 );
        }
        else if( keySymbol == "Left" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() - 1 );
        }
        else if( keySymbol == "Right" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + 1 );
        }
    }
    break;

    default:
    break;
    }
}
}