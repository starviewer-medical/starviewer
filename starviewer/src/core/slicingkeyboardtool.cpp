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
#include "volume.h"
#include "patient.h"
#include "mathtools.h"

// Qt
#include <QTimer>

// Vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingKeyboardTool::SlicingKeyboardTool(QViewer *viewer, QObject *parent) : SlicingTool(viewer, parent)
{
    m_toolName = "SlicingKeyboardTool";
    
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(10); // Just enough to catch all accumulated keyboard events.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    
    reassignAxis();
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
    delete m_timer;
}

void SlicingKeyboardTool::handleEvent(unsigned long eventID)
{
    //PERFORMANCE: configuration read on every little event...
    if (readConfiguration()) 
    { // configuration changed
        reassignAxis();
    }
    
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
    
    readConfiguration();
    
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

void SlicingKeyboardTool::timeout()
{
    int upDown = m_keyAccumulator_up - m_keyAccumulator_down;
    int rightLeft = m_keyAccumulator_right - m_keyAccumulator_left;
    int plusMinus = m_keyAccumulator_plus - m_keyAccumulator_minus;
    
    if (upDown != 0)
    {
        bool loop = false;
        loop = loop || (getMode(MAIN_AXIS) == SlicingMode::Slice && m_config_sliceScrollLoop);
        loop = loop || (getMode(MAIN_AXIS) == SlicingMode::Phase && m_config_phaseScrollLoop);
        scroll(upDown, MAIN_AXIS, loop);
    }
    
    if (rightLeft != 0)
    {
        bool loop = false;
        loop = loop || (getMode(SECONDARY_AXIS) == SlicingMode::Slice && m_config_sliceScrollLoop);
        loop = loop || (getMode(SECONDARY_AXIS) == SlicingMode::Phase && m_config_phaseScrollLoop);
        scroll(rightLeft, SECONDARY_AXIS, loop);
    }
    
    if (plusMinus != 0)
    {
        scroll(plusMinus, MAIN_AXIS, false, true);
    }
    
    m_keyAccumulator_up = 0;
    m_keyAccumulator_down = 0;
    m_keyAccumulator_left = 0;
    m_keyAccumulator_right = 0;
    m_keyAccumulator_plus = 0;
    m_keyAccumulator_minus = 0;
}

bool SlicingKeyboardTool::readConfiguration()
{
    Settings settings;
    bool changed = false;
    bool readValue = false;
    
    readValue = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
    changed = changed || readValue != m_config_sliceScrollLoop;
    m_config_sliceScrollLoop = readValue;
    
    readValue = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
    changed = changed || readValue != m_config_phaseScrollLoop;
    m_config_phaseScrollLoop = readValue;
    
    return changed;
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
    m_keyAccumulator_up++;
    m_timer->start();
}

void SlicingKeyboardTool::onDownPress()
{
    m_keyAccumulator_down++;
    m_timer->start();
}

void SlicingKeyboardTool::onLeftPress()
{
    m_keyAccumulator_left++;
    m_timer->start();
}

void SlicingKeyboardTool::onRightPress()
{
    m_keyAccumulator_right++;
    m_timer->start();
}

void SlicingKeyboardTool::onPlusPress()
{
    m_keyAccumulator_plus++;
    m_timer->start();
}

void SlicingKeyboardTool::onMinusPress()
{
    m_keyAccumulator_minus++;
    m_timer->start();
}

double SlicingKeyboardTool::scroll(double increment, unsigned int axis, bool scrollLoopEnabled, bool volumeScrollEnabled)
{
    double unusedIncrement = incrementLocation(axis, increment);
    // Increment should be 0, or at least something inside the [-0.5,0.5] (because of rounding to nearest slice).
    // When different, means a limit is reached.
    
    if (unusedIncrement < -0.5 -MathTools::Epsilon)
    { // Lower limit reached
        if (scrollLoopEnabled)
        {
            unusedIncrement = setLocation(axis, getMaximum(axis));
        }
        else if (volumeScrollEnabled)
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
        if (scrollLoopEnabled)
        {
            unusedIncrement = setLocation(axis, getMinimum(axis));
        }
        else if (volumeScrollEnabled)
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

}
