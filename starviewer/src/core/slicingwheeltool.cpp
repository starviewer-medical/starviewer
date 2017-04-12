/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "slicingwheeltool.h"

#include "q2dviewer.h"
#include "settings.h"
#include "coresettings.h"
#include "volume.h"
#include "patient.h"
#include "mathtools.h"
#include "logging.h"

// Std
#include <cmath>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

// Qt
#include <QTimer>

namespace udg {

SlicingWheelTool::SlicingWheelTool(QViewer *viewer, QObject *parent)
    : SlicingTool(viewer, parent)
{
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_toolName = "SlicingWheelTool";
    
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    
    reassignAxis();
}

SlicingWheelTool::~SlicingWheelTool()
{
    delete m_timer;
}

void SlicingWheelTool::handleEvent(unsigned long eventID)
{
    if (eventID == vtkCommand::MouseWheelForwardEvent)
    {
        onWheelMoved(m_2DViewer->getWheelAngleDelta().y());
    }
    else if (eventID == vtkCommand::MouseWheelBackwardEvent)
    {
        onWheelMoved(m_2DViewer->getWheelAngleDelta().y());
    }
    else if (eventID == vtkCommand::MiddleButtonPressEvent)
    {
        onMiddleButtonPress();
    }
    else if (eventID == vtkCommand::MiddleButtonReleaseEvent)
    {
        onMiddleButtonRelease();
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

void SlicingWheelTool::reassignAxis()
{
    setNumberOfAxes(2);
    bool sliceable = getRangeSize(SlicingMode::Slice) > 1;
    bool phaseable = getRangeSize(SlicingMode::Phase) > 1;
    
    {
        Settings settings;
        m_config_sliceScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
        m_config_phaseScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
        m_config_volumeScroll = settings.getValue(CoreSettings::EnableQ2DViewerWheelVolumeScroll).toBool();
    }
    
    if (sliceable && phaseable) 
    {
        setMode(MAIN_AXIS, SlicingMode::Slice);
        setMode(SECONDARY_AXIS, SlicingMode::Phase);
    }
    else if (sliceable && !phaseable)
    {
        setMode(MAIN_AXIS, SlicingMode::Slice);
        setMode(SECONDARY_AXIS, SlicingMode::Slice);
    }
    else if (!sliceable && phaseable)
    {
        setMode(MAIN_AXIS, SlicingMode::Phase);
        setMode(SECONDARY_AXIS, SlicingMode::Phase);
    }

    DEBUG_LOG("********* AXIS REASSING");
    beginScroll();
}

double SlicingWheelTool::scroll(double increment)
{
    double unusedIncrement = incrementLocation(m_currentAxis, increment);
    // Increment should be 0, or at least something inside the [-0.5,0.5] (because of rounding to nearest slice).
    // When different, means a limit is reached.
    
    
    if (unusedIncrement < -0.5 -MathTools::Epsilon)
    { // Lower limit reached
        if (m_scrollLoop)
        {
            unusedIncrement = setLocation(m_currentAxis, getMaximum(m_currentAxis));
        }
        else if (m_volumeScroll)
        {
            //NOTE: Evaluation lazyness used to check null pointers before they are used.
            //HACK: To avoid searching for a dummy volume that would not be found. getLocation (used by incrementLocation) does not expect this.
            if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient() && m_2DViewer->getMainInput()->getPatient()->getVolumesList().indexOf(m_2DViewer->getMainInput()) >= 0) 
            {
                if (getLocation(SlicingMode::Volume) > getMinimum(SlicingMode::Volume) +MathTools::Epsilon)
                { // Not at first volume
                    m_volumeInitialPositionToMaximum = true;
                    unusedIncrement = incrementLocation(SlicingMode::Volume, -1);
                }
            }
        }
    }
    else if (unusedIncrement > +0.5 +MathTools::Epsilon)
    { // Upper limit reached
        if (m_scrollLoop)
        {
            unusedIncrement = setLocation(m_currentAxis, getMinimum(m_currentAxis));
        }
        else if (m_volumeScroll)
        {
            //NOTE: Evaluation lazyness used to check null pointers before they are used.
            //HACK: To avoid searching for a dummy volume that would not be found. getLocation (used by incrementLocation) does not expect this.
            if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient() && m_2DViewer->getMainInput()->getPatient()->getVolumesList().indexOf(m_2DViewer->getMainInput()) >= 0) 
            {
                if (getLocation(SlicingMode::Volume) < getMaximum(SlicingMode::Volume) -MathTools::Epsilon)
                { // Not at the last volume
                    m_volumeInitialPositionToMaximum = false;
                    unusedIncrement = incrementLocation(SlicingMode::Volume, +1);
                }
            }
        }
    }
    return unusedIncrement;
}

void SlicingWheelTool::beginScroll()
{
    DEBUG_LOG("*********BEGIN SCROLL");
    m_currentAxis = m_ctrlPressed != m_middleButtonToggle ? SECONDARY_AXIS : MAIN_AXIS;
    m_increment = 0;
    m_scrollLoop = false;
    m_scrollLoop = m_scrollLoop || (m_config_sliceScrollLoop && getMode(m_currentAxis) == SlicingMode::Slice);
    m_scrollLoop = m_scrollLoop || (m_config_phaseScrollLoop && getMode(m_currentAxis) == SlicingMode::Phase);
    m_volumeScroll = m_config_volumeScroll && m_currentAxis == MAIN_AXIS;
}


void SlicingWheelTool::onWheelMoved(int angleDelta)
{
    m_timer->stop();
    if (!m_ignoreWheelMovement)
    {
        m_increment += angleDelta / 120.0;
        updateCursorIcon(m_increment);
        m_increment = scroll(m_increment);
        
        if (m_increment > +0.5 +MathTools::Epsilon || m_increment < -0.5 -MathTools::Epsilon)
        { // Upper or lower limit reached
            updateCursorIcon(m_increment);
            beginScroll();
        }
    }
    m_timer->start(150 + 75); // human vision reaction time plus a margin
}


void SlicingWheelTool::timeout()
{
    DEBUG_LOG("********* TIMER TRIGGERED");
    unsetCursorIcon();
    beginScroll();
}


void SlicingWheelTool::onCtrlPress()
{
    m_ctrlPressed = true;
    beginScroll();
}

void SlicingWheelTool::onCtrlRelease()
{
    m_ctrlPressed = false;
    beginScroll();
}

void SlicingWheelTool::onMiddleButtonPress()
{
    m_ignoreWheelMovement = true;
}

void SlicingWheelTool::onMiddleButtonRelease()
{
    m_ignoreWheelMovement = false;
    
    m_middleButtonToggle = !m_middleButtonToggle;
    beginScroll();
}

void SlicingWheelTool::updateCursorIcon(double increment)
{
    int index = 0;
    
    if (getMode(m_currentAxis) == SlicingMode::Phase)
    { // Phase mode
        index += 4;
    }
    
    if (increment > MathTools::Epsilon) 
    { // Positive increment
        index += 0;
        if (getLocation(m_currentAxis) >= getMaximum(m_currentAxis) - MathTools::Epsilon)
        { // Maximum limit reached
            index += 2;
        }
    }
    else if (increment < -MathTools::Epsilon) 
    { // Negative increment
        index += 1;
        if (getMinimum(m_currentAxis) + MathTools::Epsilon  >= getLocation(m_currentAxis))
        { // Minimum limit reached
            index += 2;
        }
    }
    else
    { // No increment
        index = CURSOR_ICON_DONT_UPDATE; // Do not touch the icon...
    }

    if (m_cursorIcon_lastIndex != index)
    { // Cursor modified only when it really changes, not on every little move.
        switch (index)
        {
            case 0:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-up.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 1:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-down.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 2:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-up-limit.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 3:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-down-limit.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 4:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-up.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 5:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-down.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 6:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-up-limit.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            case 7:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-down-limit.svg"))); DEBUG_LOG("********* CURSOR UPDATED"); break;
            default: break;
        }
        m_cursorIcon_lastIndex = index;
    }
}

void SlicingWheelTool::unsetCursorIcon()
{
         DEBUG_LOG("********* CURSOR UNSET");
    m_2DViewer->unsetCursor();
    m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
}


}
