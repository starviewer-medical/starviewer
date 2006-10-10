/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "move2dtool.h"
#include "q2dviewer.h"
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>

namespace udg {

Move2DTool::Move2DTool( Q2DViewer *viewer , QObject *parent, const char *name )
//  : Tool(parent)
{
    m_state = NONE;
    m_2DViewer = viewer;
}

Move2DTool::~Move2DTool()
{
}

void Move2DTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startMove();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doMove();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endMove();
    break;
    
    default:
    break;
    }
}

void Move2DTool::startMove()
{
    m_state = MOVING;
    // \TODO en comptes de fer això podríem crear crides específiques del propi visor que indirectament fan servir el vtkInteractorStyleImage
    m_2DViewer->getInteractorStyle()->StartPan();
}

void Move2DTool::doMove()
{
    if( m_state == MOVING )
        m_2DViewer->getInteractorStyle()->Pan();
}

void Move2DTool::endMove()
{
    m_state = NONE;
    m_2DViewer->getInteractorStyle()->EndPan();
}

}
