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
        // To Qt device independant pixels
        QPoint position = m_2DViewer->getEventPosition() / m_2DViewer->devicePixelRatioF();
        onMousePress(position);
    }
    else if (eventID == vtkCommand::LeftButtonReleaseEvent)
    {
        // To Qt device independant pixels
        QPoint position = m_2DViewer->getEventPosition() / m_2DViewer->devicePixelRatioF();
        onMouseRelease(position);
    }
    else if (eventID == vtkCommand::MouseMoveEvent)
    {
        // To Qt device independant pixels
        QPoint position = m_2DViewer->getEventPosition() / m_2DViewer->devicePixelRatioF();
        onMouseMove(position);
    }
}

void SlicingMouseTool::reassignAxis()
{
    setNumberOfAxes(2);
    bool sliceable = getRangeSize(SlicingMode::Slice) > 1;
    bool phaseable = getRangeSize(SlicingMode::Phase) > 1;
    // TODO: Read configuration
    m_loopEnabled = false;
    m_cursorWrapArround = false;
    
    
    if (sliceable && phaseable) 
    {
        setMode(VERTICAL_AXIS, SlicingMode::Slice);
        setMode(HORIZONTAL_AXIS, SlicingMode::Phase);
    }
    else if (sliceable && !phaseable)
    {
        setMode(VERTICAL_AXIS, SlicingMode::Slice);
        setMode(HORIZONTAL_AXIS, SlicingMode::Slice);
    }
    else if (!sliceable && phaseable)
    {
        setMode(VERTICAL_AXIS, SlicingMode::Phase);
        setMode(HORIZONTAL_AXIS, SlicingMode::Phase);
    }
}


void SlicingMouseTool::onMousePress(const QPoint &position)
{
    m_dragActive = true;
    
    beginDirectionDetection(position);
    
    if (getMode(VERTICAL_AXIS) == getMode(HORIZONTAL_AXIS))
    { // Scrolling vertically or horizontally has the same effect... so scrolling can begin immediately.
        m_currentDirection = Direction::Vertical;
        beginScroll(position);
    }
    else 
    {
        m_currentDirection = Direction::Undefined;
    }
    
    onMouseMove(position);
}

void SlicingMouseTool::onMouseMove(const QPoint &position)
{
    if (m_dragActive)
    {
        // *** Direction detection ***
        // Varies as user moves the mouse...
        Direction direction = directionDetection(m_directionStartPosition, position);
        if (direction != Direction::Undefined)
        {
            beginDirectionDetection(position);
            if (m_currentDirection != direction) 
            {
                m_currentDirection = direction;
                beginScroll(position);
            }
        }
        
        // *** Do the scrolling ***
        if (m_currentDirection != Direction::Undefined)
        {
            scroll(m_startPosition, position);
        }
        
        // *** Cursor ***
        
        // *** Mouse wrap arround ***
        
        
    }
}

void SlicingMouseTool::onMouseRelease(const QPoint &position)
{
    m_dragActive = false;
}

SlicingMouseTool::Direction SlicingMouseTool::directionDetection(const QPoint& startPosition, const QPoint& currentPosition) const
{    
    if (m_currentDirection == Direction::Undefined) 
    {
        return getDirection(startPosition, currentPosition, m_directionStepLength, 1, 1);
    }
    else if (m_currentDirection == Direction::Horizontal) 
    {
        return getDirection(startPosition, currentPosition, m_directionStepLength, 1, 0.5);
    }
    else if (m_currentDirection == Direction::Vertical)
    {
        return getDirection(startPosition, currentPosition, m_directionStepLength, 0.5, 1);
    }
    Q_ASSERT(false);
}

void SlicingMouseTool::beginDirectionDetection(const QPoint& startPosition)
{
    m_directionStartPosition = startPosition;
    m_directionStepLength = DEFAULT_DETECTION_STEP_LENGTH;
}

void SlicingMouseTool::scroll(const QPoint& startPosition, const QPoint& currentPosition)
{
    Q_ASSERT(m_currentDirection != Direction::Undefined);
    unsigned int axis; 
    double shift;
    if (m_currentDirection == Direction::Horizontal)
    {
        axis = HORIZONTAL_AXIS;
        shift = currentPosition.x() - startPosition.x();
    }
    else if (m_currentDirection == Direction::Vertical)
    {
        axis = VERTICAL_AXIS;
        shift = currentPosition.y() - startPosition.y();
    }
    
    // Calculate the new location
    double location = shift / m_stepLength;
    if (isnan(location) || isinf(location)) 
    {
        location = 0;
    }
    location += m_startLocation;
    
    // Setting the location and reacting to overflow or underflow
    double overflow = setLocation(axis, round(location));
    if (overflow > 0.001 || overflow < -0.001) 
    { // Overflow is not zero
        if (m_loopEnabled) 
        {
            //signbit returns true if negative
            setLocation(axis, signbit(overflow) ? getMaximum(axis) : getMinimum(axis)); 
        }
        beginScroll(currentPosition);
    }
    
}

void SlicingMouseTool::beginScroll(const QPoint& startPosition)
{
    Q_ASSERT(m_currentDirection != Direction::Undefined);
    unsigned int axis = m_currentDirection == Direction::Horizontal ? HORIZONTAL_AXIS : VERTICAL_AXIS;
    
    m_startLocation = getLocation(axis);
    m_startPosition = startPosition;
    
    // Step lenght determined by the viewer size.
    {
        unsigned int windowLength = std::max(1, m_currentDirection == Direction::Horizontal ? m_2DViewer->size().width() : m_2DViewer->size().height()); // Always greater than zero
        unsigned int rangeSize = std::max((unsigned int)1, getRangeSize(axis)); // Always greater than zero
        
        m_stepLength = windowLength / rangeSize;
        if (m_stepLength > DEFAULT_MAXIMUM_STEP_LENGTH)
        {
            m_stepLength = DEFAULT_MAXIMUM_STEP_LENGTH;
        }
        else if (m_stepLength < DEFAULT_MINIMUM_STEP_LENGTH)
        {
            m_stepLength = DEFAULT_MINIMUM_STEP_LENGTH;
        }
    }
}

SlicingMouseTool::Direction SlicingMouseTool::getDirection(const QPointF& startPosition, const QPointF& currentPosition, double stepLength, double xWeight, double yWeight) const
{
    double vectorX = abs((currentPosition.x() - startPosition.x()) * xWeight);
    double vectorY = abs((currentPosition.y() - startPosition.y()) * yWeight);
    double vectorLength = hypot(vectorX, vectorY);
    if (isgreater(vectorLength, stepLength))
    {
        if (isgreater(vectorX, vectorY))
        {
            return Direction::Horizontal;
        }
        else if (isgreater(vectorY, vectorX))
        {
            return Direction::Vertical;
        }
    }
    return Direction::Undefined;
}


}


