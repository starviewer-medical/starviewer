/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "tool.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "q3dviewer.h"

#include <vtkCommand.h>

namespace udg {

Tool::Tool(  QViewer *viewer, QObject *parent, const char *name)
 : QObject( parent )
{
    this->setObjectName( name );
    m_viewer = viewer;
}

Tool::~Tool()
{

}

Tool::EventIdType Tool::vtkCommandEventToToolEvent( unsigned long event )
{
    EventIdType result = None;
    switch( event )
    {
    case vtkCommand::LeftButtonPressEvent:
        result = LeftButtonDown;
    break;
    case vtkCommand::LeftButtonReleaseEvent:
        result = LeftButtonUp;
    break;
    case vtkCommand::RightButtonPressEvent:
        result = RightButtonDown;
    break;
    case vtkCommand::RightButtonReleaseEvent:
        result = RightButtonUp;
    break;
    case vtkCommand::MiddleButtonPressEvent:
        result = MiddleButtonDown;
    break;
    case vtkCommand::MiddleButtonReleaseEvent:
        result = MiddleButtonUp;
    break;
    case vtkCommand::MouseMoveEvent:
        result = MouseMove;
    break;
    case vtkCommand::MouseWheelForwardEvent:
        result = WheelForward;
    break;
    case vtkCommand::MouseWheelBackwardEvent:
        result = WheelBackward;
    break;
    case vtkCommand::KeyPressEvent:
        result = KeyDown;
    break;
    case vtkCommand::KeyReleaseEvent:
        result = KeyUp;
    break;
    default:
    break;
    }
    return result;
}

};  // end namespace udg 
