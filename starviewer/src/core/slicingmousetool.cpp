/*************************************************************************************
  Copyright (C) 2017 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2017. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "slicingmousetool.h"

#include "q2dviewer.h"

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingMouseTool::SlicingMouseTool(QViewer *viewer, QObject *parent)
: SlicingTool(viewer, parent)
{
    m_dragActive = false;
    m_verticalIsLikeHorizontal = false;
    m_direction = Direction::Undefined;
    m_lastMousePosition = QPoint(0,0);
    
    m_toolName = "SlicingMouseTool";
    reassignAxis();
}

SlicingMouseTool::~SlicingMouseTool()
{
}

void SlicingMouseTool::handleEvent(unsigned long eventID)
{
    if (eventID == vtkCommand::LeftButtonPressEvent)
    {
        onMousePress(m_2DViewer->getEventPosition());
    }
    else if (eventID == vtkCommand::LeftButtonReleaseEvent)
    {
        onMouseRelease(m_2DViewer->getEventPosition());
    }
    else if (eventID == vtkCommand::MouseMoveEvent)
    {
        onMouseMove(m_2DViewer->getEventPosition());
    }
    else if (eventID == vtkCommand::KeyPressEvent)
    {
        if (m_viewer->getInteractor()->GetControlKey())
        {
            onCtrlPress();
        }
    }
    else if (eventID == vtkCommand::KeyReleaseEvent)
    {
        if (!m_viewer->getInteractor()->GetControlKey())
        {
            onCtrlRelease();
        }
    }
}

void SlicingMouseTool::reassignAxis()
{
    setNumberOfAxes(2);
    bool sliceable = getRangeSize(SlicingMode::Slice) > 1;
    bool phaseable = getRangeSize(SlicingMode::Phase) > 1;
    if (sliceable && phaseable) 
    {
        setMode(VERTICAL_AXIS, SlicingMode::Slice);
        setMode(HORIZONTAL_AXIS, SlicingMode::Phase);
    }
    else if (sliceable && !phaseable)
    {
        setMode(VERTICAL_AXIS, SlicingMode::Slice);
        setMode(HORIZONTAL_AXIS, SlicingMode::None);
    }
    else if (!sliceable && phaseable)
    {
        setMode(VERTICAL_AXIS, SlicingMode::None);
        setMode(HORIZONTAL_AXIS, SlicingMode::Phase);
    }
}


void SlicingMouseTool::onMousePress(const QPoint &position)
{
    m_dragActive = true;
    m_direction = Direction::Undefined;
    m_lastMousePosition = position;
    onMouseMove(position);
}

void SlicingMouseTool::onMouseMove(const QPoint &position)
{
    if (m_dragActive)
    {
        if (m_direction == Direction::Undefined) // Keep detecting direction
        {
            //TODO: Temporary code.
            incrementLocation(VERTICAL_AXIS, (m_lastMousePosition.y() - position.y()) / 5);
            incrementLocation(HORIZONTAL_AXIS, (m_lastMousePosition.x() - position.x()) / 5);
            
        }
        else {
            
        }
    }
    m_lastMousePosition = position;
}

void SlicingMouseTool::onMouseRelease(const QPoint &position)
{
    onMouseMove(position);
    m_lastMousePosition = position;
    m_direction = Direction::Undefined;
    m_dragActive = false;
}

void SlicingMouseTool::onCtrlPress()
{
    m_verticalIsLikeHorizontal = true;
}

void SlicingMouseTool::onCtrlRelease()
{
    m_verticalIsLikeHorizontal = false;
}


}
