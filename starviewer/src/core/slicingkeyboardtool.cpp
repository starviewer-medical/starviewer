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

#include "slicingkeyboardtool.h"

#include "q2dviewer.h"
#include "settings.h"
#include "coresettings.h"

// Vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingKeyboardTool::SlicingKeyboardTool(QViewer *viewer, QObject *parent)
 : SlicingTool(viewer, parent)
{
    m_toolName = "SlicingKeyboardTool";
    reassignAxis();
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
}

void SlicingKeyboardTool::handleEvent(unsigned long eventID)
{
    if (eventID == vtkCommand::KeyPressEvent)
    {
        QString keySymbol = m_2DViewer->getInteractor()->GetKeySym();
        if (keySymbol == "Home")
        {
            onHomePress();
        }
        else if (keySymbol == "End")
        {
            onEndPress();
        }
        else if (keySymbol == "Up")
        {
            onUpPress();
        }
        else if (keySymbol == "Down")
        {
            onDownPress();
        }
        else if (keySymbol == "Left")
        {
            onLeftPress();
        }
        else if (keySymbol == "Right")
        {
            onRightPress();
        }
        else if (keySymbol == "plus")
        {
            onPlusPress();
        }
        else if (keySymbol == "minus")
        {
            onMinusPress();
        }
    }
}

void SlicingKeyboardTool::reassignAxis()
{
    setNumberOfAxes(2);
    bool sliceable = getRangeSize(SlicingMode::Slice) > 1;
    bool phaseable = getRangeSize(SlicingMode::Phase) > 1;
    
    {
        Settings settings;
        m_sliceScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
        m_phaseScroolLoop = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
    }
    
    if (sliceable && phaseable) 
    {
        setMode(MAIN_AXIS, SlicingMode::Slice);
        setMode(SECONDARY_AXIS, SlicingMode::Phase);
    }
    else if (sliceable && !phaseable)
    {
        setMode(MAIN_AXIS, SlicingMode::Slice);
        setMode(SECONDARY_AXIS, SlicingMode::None);
    }
    else if (!sliceable && phaseable)
    {
        setMode(MAIN_AXIS, SlicingMode::Phase);
        setMode(SECONDARY_AXIS, SlicingMode::Phase);
    }
    else
    {
        setMode(MAIN_AXIS, SlicingMode::Slice);
        setMode(SECONDARY_AXIS, SlicingMode::None);
    }

}

void SlicingKeyboardTool::onHomePress()
{
    setLocation(MAIN_AXIS, getMinimum(MAIN_AXIS));
}

void SlicingKeyboardTool::onEndPress()
{
    setLocation(MAIN_AXIS, getMaximum(MAIN_AXIS));
}

void SlicingKeyboardTool::onUpPress()
{
    incrementLocation(MAIN_AXIS, 1);
}

void SlicingKeyboardTool::onDownPress()
{
    incrementLocation(MAIN_AXIS, -1);
}

void SlicingKeyboardTool::onLeftPress()
{
    incrementLocation(SECONDARY_AXIS, -1);
}

void SlicingKeyboardTool::onRightPress()
{
    incrementLocation(SECONDARY_AXIS, 1);
}

void SlicingKeyboardTool::onPlusPress()
{
    incrementLocationWithVolumesLoop(1);
}

void SlicingKeyboardTool::onMinusPress()
{
    incrementLocationWithVolumesLoop(-1);
}

void SlicingKeyboardTool::incrementLocationWithVolumesLoop(int shift)
{
    int overflow = incrementLocation(MAIN_AXIS, shift);
    if (overflow != 0)
    {
        overflow = overflow > 0 ? 1 : -1; // Only -1 or 1 values allowed.
        m_volumeInitialPosition = overflow > 0 ? ChangeSliceQViewerCommand::SlicePosition::MinimumSlice : ChangeSliceQViewerCommand::SlicePosition::MaximumSlice;
        int volumeOverflow = incrementLocation(SlicingMode::Volume, overflow);
        if (volumeOverflow != 0) // At first or last volume
        {
            if (volumeOverflow > 0)
            { // Arrived to last volume
                m_volumeInitialPosition = ChangeSliceQViewerCommand::SlicePosition::MinimumSlice;
                setLocation(SlicingMode::Volume, getMinimum(SlicingMode::Volume));
            }
            else
            { // Arrived to first volume
                m_volumeInitialPosition = ChangeSliceQViewerCommand::SlicePosition::MaximumSlice;
                setLocation(SlicingMode::Volume, getMaximum(SlicingMode::Volume));
            }
        }
    }
}

}
