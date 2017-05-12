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

#ifndef UDGSLICINGWHEELTOOL_H
#define UDGSLICINGWHEELTOOL_H

#include "slicingtool.h"

class QTimer;

namespace udg {

/**
\brief Scroll tool for mouse wheels and touchpads aware of delta angle feature.

This tool handles the mouse scrolling events in the vertical direction.

Axes assignation criterion defaults to slices on the main axis and phases on the secondary axis. When only phases or slices exist in a volume, both axes are set 
to the same scrolling mode.

There are two ways to interact with the secondary axis:
    - Holding control key.
    - Middle button click toggle.

This tool honours phase and slice loop settings when a limit is reached. Another honoured setting is to jump to the next or previous volume when a limit is 
reached.

The tool is aware of the delta angle of the movements which enhance the user experience and allow solving the problem of event accumulation (when UI is blocked 
Qt accumulates the angular distance). When a classical mouse wheel is used, each step is considered to have 120 degrees.

Because the input device supports alpha angles, the implementation uses floating point numbers.
    - Interval comparisons are made with epsillons.
    - Unused distance in rounding to the nearest slice is not discarded.
    - The increment unused when reaching a limit is not discarded, it is used when scroll loop is enabled.

This tool uses a collection of cursor icons. See cursorIcon() for more details.

Note that a timer is used to unset the cursor icon and reset the remaining increment (may happen with delta angle wheel events) to zero.
 */
class SlicingWheelTool : public SlicingTool {
Q_OBJECT
public:
    explicit SlicingWheelTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingWheelTool();

    /// \brief Triggers wheel move event methods.
    virtual void handleEvent(unsigned long eventID) override;

public slots:
    /// \brief Assigns the axes modes for the current volume. A reset is performed (beginScroll()).
    virtual void reassignAxes() override;
    
private slots:
    /// \brief Zeroes the increments and unsets cursor icon.
    void resetTool();
    
private:
    /// \name Event handling
    //@{
    /** \brief Process angular wheel move event.
     * 
     * Scrolls the current axis, and the unused increment angle is accumulated.
     * 
     * Sets cursor icon with the current accumulated angular increment (even if it's little and produces no slice change).
     * 
     * If a limit is reached and closed loop is disabled, when the unused increment exceeds 0.5 or -0.5 the accumulated increment angle is set to zero again. This gives an immediate feedback to an opposite direction turn (Otherwise the user should scroll back all the increment before observing an slice change).
     * 
     * \see cursorIcon()
     * \see scroll()
     * \see beginScroll()
     * 
     * \param angleDelta Degrees turned by the wheel. May be negative. 
     */
    void onWheelMoved(int angleDelta);
    /** \brief Sets a variable to reflect the status and resets the scroll.
     * \see beginScroll()
     */
    void onCtrlPress();
    /// \see onCtrlRelease()
    void onCtrlRelease();
    /// \see onCtrlRelease()
    void onMiddleButtonPress();
    /// \see onCtrlRelease()
    void onMiddleButtonRelease();
    //@}
    
    /** \brief Sets the cursor icon using the available information.
     * 
     * Setting the cursor requires the following information:
     *     - The \p increment parameter defines the sign of the movement.
     *     - Current slice.
     *     - Minimum and maximum values. (For the limit reached icons)
     *     - Type of the current axis. (Phases or slices)
     * 
     * With that information any combination of these situations can be expressed:
     *     - Increment or decrement movement.
     *     - Upper or lower limit reached.
     *     - Scrolling of phases or slices.
     * 
     * The method implementation calculates an index used to choose an icon from a hard-coded list.
     * 
     * In order to avoid futile updates, the last index of the set cursor set is stored. The cursor is updated only when the index changes.
     */
    void cursorIcon(double increment);
    /// \brief Unsets the cursor icon and updates the index of the last cursor.
    void unsetCursorIcon();
    
    /** \brief Scrolls the current axis.
     * 
     * The current axis is incremented by \p increment.
     * The current behavior of the tool is defined using the variables inside the "Current scroll status" group.
     * When a upper or lower limit is reached a loop or a jump to the next or previous volume might be performed.
     * 
     * \pre Having a started the scrolling with beginScroll().
     * \return The amount of unused increment.
     */
    double scroll(double increment);
    /** \brief Resets the current scroll and begins a new one.
     * 
     * A new scroll is began and its behaviour is defined by:
     *     - Status of ctrl key and middle button toogle that choose the axis to scroll.
     *     - Loop configuration for the type of the selected axis.
     *     - Volume jump configuration.
     * 
     * When called, the configuration is read.
     * 
     * The accumulated increment is set to zero.
     * 
     * This method shall be used when the movement finishes or when the behavior of the tool has to be changed.
     */
    void beginScroll();

private:
    QTimer* m_timer;

    /// \brief Last cursor icon index used, this variable is used to avoid futile cursor updates.
    int m_cursorIcon_lastIndex;
    /// \brief Default value to avoid a cursor icon change.
    static constexpr int CursorIconDontUpdate = -1;
    
    /// \brief Some mouses produce unwanted scrolls when the wheel is clicked. This is used to cancel them until the wheel is released.
    bool m_ignoreWheelMovement;
    bool m_ctrlPressed;
    bool m_middleButtonToggle;
    
    ///\name Current scroll status
    //@{
    /// \brief Accumulator for angular distance.
    double m_increment;
    /// \brief Axis where the current increment is applied.
    unsigned int m_currentAxis;
    /// \brief Closed loop scroll enabled for the current axis.
    bool m_scrollLoop;
    /// \brief Jump to next or previous volume enabled for the current axis.
    bool m_volumeScroll;
    //@}
    
    static constexpr unsigned int MainAxis = 0;
    static constexpr unsigned int SecondaryAxis = 1;
};

}

#endif //UDGSLICINGWHEELTOOL_H
