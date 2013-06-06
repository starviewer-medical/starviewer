#include "genericdistancetool.h"

#include <vtkCommand.h>
#include "q2dviewer.h"

namespace udg {

GenericDistanceTool::GenericDistanceTool(QViewer *viewer, QObject *parent)
 : MeasurementTool(viewer, parent)
{
    m_hasSharedData = false;
}

GenericDistanceTool::~GenericDistanceTool()
{
}

void GenericDistanceTool::handleEvent(long unsigned eventID)
{
    if (!m_2DViewer || !m_2DViewer->getInput())
    {
        return;
    }
    
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handleLeftButtonPress();
            break;

        case vtkCommand::MouseMoveEvent:
            handleMouseMove();
            break;

        case vtkCommand::KeyPressEvent:
            handleKeyPress();
            break;
    }
}

} // End namespace udg

