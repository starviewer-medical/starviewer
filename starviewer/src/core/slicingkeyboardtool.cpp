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

SlicingKeyboardTool::SlicingKeyboardTool(QViewer *viewer, QObject *parent)
: SlicingTool(viewer, parent), m_keyAccumulator{0, 0, 0, 0, 0, 0}
{
    m_toolName = "SlicingKeyboardTool";
    
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(10); // Just enough to catch all accumulated keyboard events.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(processAccumulation()));
    
    reassignAxes();
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
    delete m_timer;
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

void SlicingKeyboardTool::reassignAxes()
{
    setNumberOfAxes(2);
    bool sliceable = getRangeSize(SlicingMode::Slice) > 1;
    bool phaseable = getRangeSize(SlicingMode::Phase) > 1;
    
    if (sliceable && phaseable) 
    {
        setMode(MainAxis, SlicingMode::Slice);
        setMode(SecondaryAxis, SlicingMode::Phase);
    }
    else if (sliceable && !phaseable)
    {
        setMode(MainAxis, SlicingMode::Slice);
        setMode(SecondaryAxis, SlicingMode::None);
    }
    else if (!sliceable && phaseable)
    {
        setMode(MainAxis, SlicingMode::Phase);
        setMode(SecondaryAxis, SlicingMode::Phase);
    }
    else
    {
        setMode(MainAxis, SlicingMode::Slice);
        setMode(SecondaryAxis, SlicingMode::None);
    }

}

void SlicingKeyboardTool::processAccumulation()
{
    Settings settings;
    bool configSliceScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
    bool configPhaseScrollLoop = settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
    
    int upDown = m_keyAccumulator.up - m_keyAccumulator.down;
    int rightLeft = m_keyAccumulator.right - m_keyAccumulator.left;
    int plusMinus = m_keyAccumulator.plus - m_keyAccumulator.minus;
    
    if (upDown != 0)
    {
        bool loop = false;
        loop = loop || (getMode(MainAxis) == SlicingMode::Slice && configSliceScrollLoop);
        loop = loop || (getMode(MainAxis) == SlicingMode::Phase && configPhaseScrollLoop);
        scroll(upDown, MainAxis, loop);
    }
    
    if (rightLeft != 0)
    {
        bool loop = false;
        loop = loop || (getMode(SecondaryAxis) == SlicingMode::Slice && configSliceScrollLoop);
        loop = loop || (getMode(SecondaryAxis) == SlicingMode::Phase && configPhaseScrollLoop);
        scroll(rightLeft, SecondaryAxis, loop);
    }
    
    if (plusMinus != 0)
    {
        scroll(plusMinus, MainAxis, false, true);
    }
    
    m_keyAccumulator.up = 0;
    m_keyAccumulator.down = 0;
    m_keyAccumulator.left = 0;
    m_keyAccumulator.right = 0;
    m_keyAccumulator.plus = 0;
    m_keyAccumulator.minus = 0;
}

void SlicingKeyboardTool::onHomePress()
{
    setLocation(MainAxis, getMinimum(MainAxis));
}

void SlicingKeyboardTool::onEndPress()
{
    setLocation(MainAxis, getMaximum(MainAxis));
}

void SlicingKeyboardTool::onUpPress()
{
    m_keyAccumulator.up++;
    m_timer->start();
}

void SlicingKeyboardTool::onDownPress()
{
    m_keyAccumulator.down++;
    m_timer->start();
}

void SlicingKeyboardTool::onLeftPress()
{
    m_keyAccumulator.left++;
    m_timer->start();
}

void SlicingKeyboardTool::onRightPress()
{
    m_keyAccumulator.right++;
    m_timer->start();
}

void SlicingKeyboardTool::onPlusPress()
{
    m_keyAccumulator.plus++;
    m_timer->start();
}

void SlicingKeyboardTool::onMinusPress()
{
    m_keyAccumulator.minus++;
    m_timer->start();
}

double SlicingKeyboardTool::scroll(double increment, unsigned int axis, bool scrollLoopEnabled, bool volumeScrollEnabled)
{
    double unusedIncrement = incrementLocation(axis, increment);
    // Increment should be 0, or at least something inside the [-0.5,0.5] (because of rounding to nearest slice).
    // When different, means a limit is reached.
    
    if (unusedIncrement < -0.5 -MathTools::Epsilon)
    {
        // Lower limit reached
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
                {
                    // Not at first volume
                    m_volumeInitialPositionToMaximum = true;
                    unusedIncrement = incrementLocation(SlicingMode::Volume, -1);
                }
            }
        }
    }
    else if (unusedIncrement > +0.5 +MathTools::Epsilon)
    {
        // Upper limit reached
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
