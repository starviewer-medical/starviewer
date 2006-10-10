/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "zoom2dtool.h"
#include "q2dviewer.h"
#include <vtkInteractorStyleImage.h> // per controlar el window level
#include <vtkCommand.h>

namespace udg {

Zoom2DTool::Zoom2DTool( Q2DViewer *viewer , QObject *parent, const char *name )
//  : Tool(parent)
{
    m_state = NONE;
    m_2DViewer = viewer;
}

Zoom2DTool::~Zoom2DTool()
{
}

void Zoom2DTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
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

void Zoom2DTool::startZoom()
{
    m_state = ZOOMING;
    // \TODO en comptes de fer això podríem crear crides específiques del propi visor que indirectament fan servir el vtkInteractorStyleImage
    m_2DViewer->getInteractorStyle()->StartDolly();
}

void Zoom2DTool::doZoom()
{
    if( m_state == ZOOMING )
        m_2DViewer->getInteractorStyle()->Dolly();
}

void Zoom2DTool::endZoom()
{
    m_state = NONE;
    m_2DViewer->getInteractorStyle()->EndUniformScale();
}

}
