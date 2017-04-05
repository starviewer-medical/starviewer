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

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingWheelTool::SlicingWheelTool(QViewer *viewer, QObject *parent)
    : SlicingTool(viewer, parent)
{
    m_scrollDisabled = false;
    m_ctrlPressed = false;
    m_middleButtonToggled = false;
    
    m_toolName = "SlicingWheelTool";
    reassignAxis();
}

SlicingWheelTool::~SlicingWheelTool()
{

}

void SlicingWheelTool::handleEvent(unsigned long eventID)
{
    if (eventID == vtkCommand::MouseWheelForwardEvent)
    {
        scroll(1);
    }
    else if (eventID == vtkCommand::MouseWheelBackwardEvent)
    {
        scroll(-1);
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
        m_sliceScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
        m_phaseScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
        m_volumeScroll = settings.getValue(CoreSettings::EnableQ2DViewerWheelVolumeScroll).toBool();
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
}

void SlicingWheelTool::scroll(int steps)
{
    if (!m_scrollDisabled)
    {
        unsigned int axis;
        if (m_ctrlPressed != m_middleButtonToggled) 
        { // When true, do to alternative mode
            axis = SECONDARY_AXIS; 
        }
        else
        {
            axis = MAIN_AXIS;
        }
        
        double overflow = incrementLocation(axis, steps);
        
        // *** Closed scroll loop of phases or slices ***
        bool scrollLoopEnabled = false;
        scrollLoopEnabled = scrollLoopEnabled || (m_sliceScrollLoop && getMode(axis) == SlicingMode::Slice);
        scrollLoopEnabled = scrollLoopEnabled || (m_phaseScrollLoop && getMode(axis) == SlicingMode::Phase);
        if (overflow > 0.001 || overflow < -0.001) 
        { // Overflow is not zero
            if (scrollLoopEnabled) 
            {
                //signbit returns true if negative
                overflow = setLocation(axis, signbit(overflow) ? getMaximum(axis) : getMinimum(axis));
            }
        }
        
        // *** Change to next volume at the limits ***
        
        if (m_volumeScroll && axis == MAIN_AXIS)
        {
            //NOTE: Evaluation lazyness used to check null pointers before they are used.
            //HACK: To avoid searching for a dummy volume that would not be found. getLocation (used by incrementLocation) does not expect this.
            if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient() && m_2DViewer->getMainInput()->getPatient()->getVolumesList().indexOf(m_2DViewer->getMainInput()) >= 0) 
            {
                if (overflow > 0.001 && getLocation(SlicingMode::Volume) < getMaximum(SlicingMode::Volume))
                { // Maximum limit reached
                    m_volumeInitialPositionToMaximum = false;
                    incrementLocation(SlicingMode::Volume, 1);
                }
                else if (overflow < -0.001 && getLocation(SlicingMode::Volume) > getMinimum(SlicingMode::Volume))
                { // Minimum limit reached
                    m_volumeInitialPositionToMaximum = true;
                    incrementLocation(SlicingMode::Volume, -1);
                }
            }
        }
        
        updateCursorIcon(axis, steps);
        
    }
}

void SlicingWheelTool::onCtrlPress()
{
    m_ctrlPressed = true;
}

void SlicingWheelTool::onCtrlRelease()
{
    m_ctrlPressed = false;
}

void SlicingWheelTool::onMiddleButtonPress()
{
    m_scrollDisabled = true;
}

void SlicingWheelTool::onMiddleButtonRelease()
{
    m_middleButtonToggled = !m_middleButtonToggled;
    m_scrollDisabled = false;
    unsetCursorIcon();
}

void SlicingWheelTool::updateCursorIcon(unsigned int axis, double increment)
{
    int index = 0;
    
    if (getMode(axis) == SlicingMode::Phase)
    { // Phase mode
        index += 4;
    }
    
    if (increment > 0.001) 
    { // Positive increment
        index += 0;
        if (getLocation(axis) >= getMaximum(axis) - 0.001)
        { // Maximum limit reached
            index += 2;
        }
    }
    else if (increment < -0.001) 
    { // Negative increment
        index += 1;
        if (getMinimum(axis) + 0.001  >= getLocation(axis))
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
            case 0:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-up.svg"))); break;
            case 1:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-down.svg"))); break;
            case 2:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-up-limit.svg"))); break;
            case 3:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-slice-down-limit.svg"))); break;
            case 4:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-up.svg"))); break;
            case 5:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-down.svg"))); break;
            case 6:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-up-limit.svg"))); break;
            case 7:  m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/wheel-phase-down-limit.svg"))); break;
            default: break;
        }
        m_cursorIcon_lastIndex = index;
    }
}

void SlicingWheelTool::unsetCursorIcon()
{
    m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
}

}
