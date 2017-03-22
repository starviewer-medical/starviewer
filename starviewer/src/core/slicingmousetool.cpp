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
#include "logging.h"

// Qt
#include <QtMath>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingMouseTool::SlicingMouseTool(QViewer *viewer, QObject *parent)
: SlicingTool(viewer, parent)
{
    m_dragActive = false;
    m_verticalIsLikeHorizontal = false;
    m_direction = CardinalDirection::Undefined;
    
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

SlicingMouseTool::CardinalDirection SlicingMouseTool::getDirection(double radians)
{
    constexpr double deg000 = 0;
    constexpr double deg030 = M_PI/6;
    constexpr double deg060 = M_PI/3;
    constexpr double deg120 = (2*M_PI)/3;
    constexpr double deg150 = (5*M_PI)/6;
    constexpr double deg210 = (7*M_PI)/6;
    constexpr double deg240 = (4*M_PI)/3;
    constexpr double deg300 = (5*M_PI)/3;
    constexpr double deg330 = (11*M_PI)/6;
    constexpr double deg360 = 2*M_PI;
    
    radians = fmod(radians, deg360);
    radians += radians >= 0 ? 0 : M_PI*2;
    
    if (deg030 >= radians && radians >= deg000) 
    {
        return CardinalDirection::East;
    }
    else if (deg060 > radians && radians > deg030)
    {
        return CardinalDirection::NorthEast;
    }
    else if (deg120 >= radians && radians >= deg060)
    {
        return CardinalDirection::North;
    }
    else if (deg150 > radians && radians > deg120)
    {
        return CardinalDirection::NorthWest;
    }
    else if (deg210 >= radians && radians >= deg150)
    {
        return CardinalDirection::West;
    }
    else if (deg240 > radians && radians > deg210)
    {
        return CardinalDirection::SouthWest;
    }
    else if (deg300 >= radians && radians >= deg240)
    {
        return CardinalDirection::South;
    }
    else if (deg330 > radians && radians > deg300)
    {
        return CardinalDirection::SouthEast;
    }
    else if (deg360 >= radians && radians >= deg330) 
    {
        return CardinalDirection::East;
    }
    return CardinalDirection::Undefined;
}

void SlicingMouseTool::onMousePress(const QPoint &position)
{
    m_dragActive = true;
    
    m_direction = CardinalDirection::Undefined;
    m_unusedDistance = 0;
    m_oldPosition = position;
    
    onMouseMove(position);
}

void SlicingMouseTool::onMouseMove(const QPoint &position)
{
    if (m_dragActive)
    {
        QPoint azimuthVector = m_oldPosition - position ; 
        double distance = hypot(azimuthVector.x(), azimuthVector.y());
        double azimuth = atan2(azimuthVector.y(), azimuthVector.x()) + M_PI;
        CardinalDirection direction = getDirection(azimuth);
        CardinalDirection oppositeDirection = getDirection(azimuth - M_PI);
        
        if (m_direction == CardinalDirection::Undefined) 
        {
            if (distance >= m_detectionDistance)
            { // Moved sufficiently to determine the direction
                m_direction = direction;
                m_oldPosition = position;
                m_unusedDistance = 0;
            }
            DEBUG_LOG(QString("UNDEFINED Azimuth: %0 Distance: %1 azv: %2 %3").arg(azimuth).arg(distance).arg(azimuthVector.x()).arg(azimuthVector.y()));
        }
        else if (m_direction == direction || m_direction == oppositeDirection) 
        {
            int sign = m_direction == oppositeDirection ? -1 : 1;
            int increment = (sign*distance + m_unusedDistance) / m_scrollDistance;
            unsigned int axisNumber = 0;
            if (m_direction == CardinalDirection::East || m_direction == CardinalDirection::West)
            {
                axisNumber = 1;
            }
            
            m_scrollDistance = 8;
            if (getRangeSize(axisNumber) < 32)
            {
                m_scrollDistance = 32;
                
            }
            else if (getRangeSize(axisNumber) < 64)
            {
                m_scrollDistance = 16;
                
            }
            
            
            if (increment) {
                incrementLocation(axisNumber, increment);
                m_unusedDistance = fmod(sign*distance, (double)m_scrollDistance);
                m_oldPosition = position;
            }
            
            
            DEBUG_LOG(QString("DIR DEFINED Azimuth: %0 Distance: %1").arg(azimuth).arg(distance));
        }
        else 
        { // User took a totally different direction.
            DEBUG_LOG(QString("GOT OUT Azimuth: %0 Distance: %1").arg(azimuth).arg(distance));
            m_unusedDistance = 0;
            m_oldPosition = position;
        }
    }
}

void SlicingMouseTool::onMouseRelease(const QPoint &position)
{
    onMouseMove(position);
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
