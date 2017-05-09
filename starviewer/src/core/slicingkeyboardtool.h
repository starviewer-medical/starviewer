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

#ifndef UDGSLICINGKEYBOARDTOOL_H
#define UDGSLICINGKEYBOARDTOOL_H

#include "slicingtool.h"

class QTimer;

namespace udg {

/**
\brief Scroll tool using keyboard keys.

The tool uses two axes used for scrolling slices and phases.

Axes:
    - __Main axis__ \n
    Scrolls slices.\n
    Scrolls phases if there's only one slice and more than one phase.
        - Home
            - Go to first
        - End
            - Go to last
        - Up
            - Increment
        - Down
            - Decrement
    - __Secondary axis__\n
    Always scrolls phases.
        - Right
            - Increment
        - Left
            - Decrement
    - __%Volume scroll__\n
    Scrolls the main axis but ignoring the axis loop setting and jumps to next or previous volume when reaching the limits.
     - Plus
         - Increment and if necessary go to next volume.
     - Minus
         - Decrement and if necessary go to previous volume.

This tool honours phase and slice loop settings when a limit is reached on the main or secondary axis.

Note that a timer hack is used in order to catch the accumulation of key events; this increases user interactivity and avoids frustration.
 */
class SlicingKeyboardTool : public SlicingTool {
Q_OBJECT
public:
    explicit SlicingKeyboardTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingKeyboardTool();
    
    /// \brief Triggers key press event methods.
    virtual void handleEvent(unsigned long eventID) override;
    
public slots:
    /// \brief Assigns the axes modes for the current volume.
    virtual void reassignAxes() override;
    
private slots:
    /** \brief Process key accumulated events.
     * 
     * Call the scroll() for each key pair with the accumulated increment. Once executed the accumulated keys are zeroed and the timer stopped (single shot timer).
     * 
     * When called, the configuration is read.
     */
    void processAccumulation();

private:
    /// \brief Reads settings and sets the m_config_* members.
    void readConfiguration();
    
    /// \name Event handling
    /// Except home and end that trigger an immediate action. Key events are accumulated to a counter.
    //@{
    
    /// \brief Puts the main axis to the minimum position.
    void onHomePress();
    /// \brief Puts the main axis to the maximum position
    void onEndPress();
    void onUpPress();
    void onDownPress();
    void onLeftPress();
    void onRightPress();
    void onPlusPress();
    void onMinusPress();
    //@}
    
    /** \brief Increases the specified axis number.
     * 
     * \param scrollLoopEnabled When a limit is reached jump at the beginning or end of the axis.
     * \param volumeScrollEnabled When a limit is reached jump to the next volume, setting the volume to the minimum or maximum slice (or phase in case the number of slices is one or less).
     * 
     * The conflictive case of setting to true scroll loop and volume scroll, scroll loop is preferred.
     * 
     * \return Amount of unused scroll increment.
     */
    double scroll(double increment, unsigned int axis = MainAxis, bool scrollLoopEnabled = false, bool volumeScrollEnabled = false);
    
    QTimer* m_timer = 0;
    
    ///\name Settings
    //@{
    bool m_config_sliceScrollLoop = false;
    bool m_config_phaseScrollLoop  = false;
    //@}
    
    /** 
     * \name Key accumulator counters
     * Store the number of keys pressed until the timeout is expired and they are processed
     */
    //@{
    int m_keyAccumulator_up = 0;
    int m_keyAccumulator_down = 0;
    int m_keyAccumulator_left = 0;
    int m_keyAccumulator_right = 0;
    int m_keyAccumulator_plus = 0;
    int m_keyAccumulator_minus = 0;
    //@}
    
    static constexpr unsigned int MainAxis = 0;
    static constexpr unsigned int SecondaryAxis = 1;
};

}

#endif //UDGSLICINGKEYBOARDTOOL_H
