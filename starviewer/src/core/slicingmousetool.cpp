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
#include "settings.h"
#include "coresettings.h"
#include "mathtools.h"

// Std
#include <cmath>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingMouseTool::SlicingMouseTool(QViewer *viewer, QObject *parent) : SlicingTool(viewer, parent)
{
    m_toolName = "SlicingMouseTool";
    reassignAxes();
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
    else if (eventID == vtkCommand::LeftButtonReleaseEvent || eventID == vtkCommand::RightButtonReleaseEvent)
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

void SlicingMouseTool::reassignAxes()
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
        setMode(HORIZONTAL_AXIS, SlicingMode::Slice);
    }
    else if (!sliceable && phaseable)
    {
        setMode(VERTICAL_AXIS, SlicingMode::Phase);
        setMode(HORIZONTAL_AXIS, SlicingMode::Phase);
    }
}

void SlicingMouseTool::readConfiguration()
{
    Settings settings;
    m_config_sliceScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();;
    m_config_phaseScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();;
    m_config_wraparound = settings.getValue(CoreSettings::EnableQ2DViewerMouseWraparound).toBool();;
}

void SlicingMouseTool::onMousePress(const QPoint &position)
{
    m_dragActive = true;
    
    readConfiguration();
    beginCursorIcon(position);
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
        // *** Mouse wrap arround ***
        if (m_config_wraparound)
        {
            // Have we made a move wrap arround on the previous movement?
            if (
                (m_wraparound_wrappedToLeft && position.x() < m_wraparound_positionBeforeWrapping.x()) ||
                (m_wraparound_wrappedToRight && position.x() > m_wraparound_positionBeforeWrapping.x()) ||
                (m_wraparound_wrappedToTop && position.y() > m_wraparound_positionBeforeWrapping.y()) ||
                (m_wraparound_wrappedToBottom && position.y() < m_wraparound_positionBeforeWrapping.y())
            )
            {
                m_wraparound_wrappedToLeft = false;
                m_wraparound_wrappedToRight = false;
                m_wraparound_wrappedToTop = false;
                m_wraparound_wrappedToBottom = false;
                beginCursorIcon(position);
                beginDirectionDetection(position);
                if (m_currentDirection != Direction::Undefined)
                {
                    beginScroll(position);
                }
            }
            
            // Do we have to do a mouse wrap arround in the next movement?
            QPoint cursor = QCursor::pos();
            const QPoint& topLeft = m_2DViewer->mapToGlobal(m_2DViewer->rect().topLeft());
            const QPoint& bottomRight = m_2DViewer->mapToGlobal(m_2DViewer->rect().bottomRight());
            
            if (cursor.x() < topLeft.x())
            {
                cursor.setX(bottomRight.x());
                m_wraparound_wrappedToRight = true;
            }
            else if (cursor.x() > bottomRight.x())
            {
                cursor.setX(topLeft.x());
                m_wraparound_wrappedToLeft = true;
            }
            
            if (cursor.y() < topLeft.y())
            {
                cursor.setY(bottomRight.y());
                m_wraparound_wrappedToBottom = true;
            }
            else if (cursor.y() > bottomRight.y())
            {
                cursor.setY(topLeft.y());
                m_wraparound_wrappedToTop = true;
            }
            
            if (m_wraparound_wrappedToLeft || m_wraparound_wrappedToRight || m_wraparound_wrappedToTop ||m_wraparound_wrappedToBottom) 
            { // Change the position of the mouseS
                m_wraparound_positionBeforeWrapping = position;
                QCursor::setPos(cursor);
            }
        }
        
        // *** Direction detection ***
        // Direction varies and gets eventually defined as user moves the mouse...
        Direction direction = directionDetection(position);
        if (direction != Direction::Undefined)
        {
            beginDirectionDetection(position);
            if (m_currentDirection != direction) 
            {
                m_currentDirection = direction;
                beginScroll(position);
            }
        }
        
        // *** Scrolling ***
        if (m_currentDirection != Direction::Undefined)
        {
            scroll(position);
        }
        
        // *** Cursor icons ***
        cursorIcon(position);
    }
}

void SlicingMouseTool::onMouseRelease(const QPoint &position)
{
    m_dragActive = false;
    m_wraparound_wrappedToLeft = false;
    m_wraparound_wrappedToRight = false;
    m_wraparound_wrappedToTop = false;
    m_wraparound_wrappedToBottom = false;
    unsetCursorIcon();
}

void SlicingMouseTool::cursorIcon(const QPoint &currentPosition)
{
    int index = 0;
    if (m_currentDirection != Direction::Undefined)
    {
        unsigned int axis;
        double positionIncrement;
        
        if (m_currentDirection == Direction::Vertical)
        {
            index += 2;
            axis = VERTICAL_AXIS;
            positionIncrement = currentPosition.y() - m_cursorIcon_lastPosition.y();
        }
        else if (m_currentDirection == Direction::Horizontal)
        {
            index += 0;
            axis = HORIZONTAL_AXIS;
            positionIncrement = currentPosition.x() - m_cursorIcon_lastPosition.x();
        }
        else { Q_ASSERT(false); }
        
        if (getMode(axis) == SlicingMode::Phase)
        { // Phase mode
            index += 8;
        }
        
        if (positionIncrement > MathTools::Epsilon) 
        { // Positive increment
            index += 1;
            if (getLocation(axis) >= getMaximum(axis) - MathTools::Epsilon)
            { // Maximum limit reached
                index += 4;
            }
            
        }
        else if (positionIncrement < -MathTools::Epsilon) 
        { // Negative increment
            index += 0;
            if (getMinimum(axis) + MathTools::Epsilon  >= getLocation(axis))
            { // Minimum limit reached
                index += 4;
            }
        }
        else
        { // No increment
            index = CURSOR_ICON_DONT_UPDATE; // Do not touch the icon...
        }
    }
    else
    {
        index = 16;
    }

    if (m_cursorIcon_lastIndex != index)
    { // Cursor modified only when it really changes, not on every little move.
        switch (index)
        {
            case 0:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-left.svg"))); break;
            case 1:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-right.svg"))); break;
            case 2:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-down.svg"))); break;
            case 3:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-up.svg"))); break;
            case 4:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-left-limit.svg"))); break;
            case 5:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-right-limit.svg"))); break;
            case 6:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-down-limit.svg"))); break;
            case 7:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-slice-up-limit.svg"))); break;
            case 8:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-left.svg"))); break;
            case 9:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-right.svg"))); break;
            case 10: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-down.svg"))); break;
            case 11: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-up.svg"))); break;
            case 12: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-left-limit.svg"))); break;
            case 13: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-right-limit.svg"))); break;
            case 14: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-down-limit.svg"))); break;
            case 15: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll-phase-up-limit.svg"))); break;    
            case 16: m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll.svg"))); break;
            default: break;
        }
        m_cursorIcon_lastIndex = index;
    }
    m_cursorIcon_lastPosition = currentPosition;
}

void SlicingMouseTool::beginCursorIcon(const QPoint &startPosition)
{
    m_cursorIcon_lastPosition = startPosition;
    m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
}

void SlicingMouseTool::unsetCursorIcon()
{
    m_2DViewer->unsetCursor();
    m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
    m_cursorIcon_lastPosition = QPoint(0,0);
}

double SlicingMouseTool::scroll(const QPoint& currentPosition)
{
    Q_ASSERT(m_currentDirection != Direction::Undefined);
    unsigned int axis; 
    double shift;
    if (m_currentDirection == Direction::Horizontal)
    {
        axis = HORIZONTAL_AXIS;
        shift = currentPosition.x() - m_startPosition.x();
    }
    else if (m_currentDirection == Direction::Vertical)
    {
        axis = VERTICAL_AXIS;
        shift = currentPosition.y() - m_startPosition.y();
    }
    
    // Calculate the destination location
    double location = shift / m_stepLength;
    if (std::isnan(location) || std::isinf(location)) 
    {
        location = 0;
    }
    location += m_startLocation;
    
    // Setting the location and reacting when reaching limits
    double newLocation = setLocation(axis, location);
    
    // Location and new location differ more than 1 (where rounding happens) this means a limit is reached.
    if (location < newLocation -0.5 -MathTools::Epsilon)
    { // Underflow
        if (m_scrollLoop)
        {
            newLocation = setLocation(axis, getMaximum(axis));
        }
        beginScroll(currentPosition);
    }
    else if (location > newLocation +0.5 +MathTools::Epsilon)
    { // Overflow
        if (m_scrollLoop)
        {
            newLocation = setLocation(axis, getMinimum(axis));
        }
        beginScroll(currentPosition);
    }
    
    return newLocation;
}

void SlicingMouseTool::beginScroll(const QPoint& startPosition)
{
    Q_ASSERT(m_currentDirection != Direction::Undefined);
    unsigned int axis = m_currentDirection == Direction::Horizontal ? HORIZONTAL_AXIS : VERTICAL_AXIS;
    
    m_startLocation = getLocation(axis);
    m_startPosition = startPosition;
    
    // Determine if closed loop scrolling must be enabled
    {
         m_scrollLoop = false;
         m_scrollLoop = m_scrollLoop || (getMode(axis) == SlicingMode::Slice && m_config_sliceScrollLoop);
         m_scrollLoop = m_scrollLoop || (getMode(axis) == SlicingMode::Phase && m_config_phaseScrollLoop);
    }
    
    // Step lenght determined by the viewer size.
    {
        unsigned int windowLength = std::max(1, m_currentDirection == Direction::Horizontal ? m_2DViewer->size().width() : m_2DViewer->size().height()); // Always greater than zero
        unsigned int rangeSize = std::max(1.0, getRangeSize(axis)); // Always greater than zero
        
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

SlicingMouseTool::Direction SlicingMouseTool::directionDetection(const QPoint& currentPosition) const
{
    double xWeight = 1;
    double yWeight = 1;
    if (m_currentDirection == Direction::Horizontal) 
    {
        xWeight = 1;
        yWeight = 0.5;
    }
    else if (m_currentDirection == Direction::Vertical)
    {
        xWeight = 0.5;
        yWeight = 1;
    }
    
    double vectorX = std::abs((currentPosition.x() - m_directionDetection_startPosition.x()) * xWeight);
    double vectorY = std::abs((currentPosition.y() - m_directionDetection_startPosition.y()) * yWeight);
    double vectorLength = std::hypot(vectorX, vectorY);
    if (vectorLength > m_directionDetection_stepLength)
    {
        if (vectorX > vectorY)
        {
            return Direction::Horizontal;
        }
        else if (vectorY > vectorX)
        {
            return Direction::Vertical;
        }
    }
    return Direction::Undefined;
}

void SlicingMouseTool::beginDirectionDetection(const QPoint& startPosition)
{
    m_directionDetection_startPosition = startPosition;
    m_directionDetection_stepLength = DEFAULT_DETECTION_STEP_LENGTH;
}

}
