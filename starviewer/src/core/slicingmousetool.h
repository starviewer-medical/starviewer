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

#ifndef UDGSLICINGMOUSETOOL_H
#define UDGSLICINGMOUSETOOL_H

#include "slicingtool.h"

// Qt
#include <QPoint>

namespace udg {

/**
\brief Movement aware scroll tool using mouse dragging.

Dragging the mouse over the viewer with the left mouse button pressed slices or phases can be scrolled.

This tool provides two axes that can be selected by making a mainly vertical or manly horizontal movement. The user selects the axis by making a strong move, so 
no accidental scrolling will happen. The tool is aware of the context like the number of slices, viewer size, etc... to adjust the required distance to scroll 
an slice.

User interaction
================

Axis selection
--------------
There are two axes, accessed by the direction of the movement:
    - __Vertical axis__\n
      Used to scroll slices.
      If there's only one slice, then scrolls phases.
    - __Horizontal axis__\n
      Used to scroll phases.
      If there's only one phase, then scrolls slices.

The active axis can be chosen with the direction of the movement and the user may change the direction during the same drag movement (no button release) as many 
times he or she wants.

In the beginning the direction is not clear until a minimum distance (DEFAULT_DETECTION_STEP_LENGTH) is traveled. Nonetheless, when both axes are scrolling the 
same thing, the vertical axis is assumed to be initially selected and thus the user can start scrolling immediately without having to travel the direction 
detection distance.

Direction detection
-------------------
The direction detection is always enabled, even when one has been found because the user is allowed to switch to the other axis during the whole drag movement.

When the scrolling direction is undefined, the user must travel a distance to detect the movement, however, once a direction is found (while scrolling) it's 
more difficult (more distance to travel) in order to switch to the other direction.

Adaptive scroll distance
------------------------
One might find series with a few or many images; adapting to those situations is important to avoid user frustration. When a direction is defined, the amount of 
pixels required to travel in order to change an slice depend on the number of slices and the viewer size.

The viewer size (width or length) is divided by the number of images of the current axis. Some situations may lead to very big or very small travel distances. 
To overcome this, the value will always be within the bounds defined by DEFAULT_MINIMUM_STEP_LENGTH and DEFAULT_MAXIMUM_STEP_LENGTH.

See beginScroll().

Mouse wraparound
----------------
When the mouse escapes the area of the viewer during the drag movement, the cursor is teleported to the opposite side. This enables the user to perform infinite 
scrolls without having to release the mouse button.

This behavior is user configurable.

Scrolling limits
----------------
When a limit is reached:
    - A loop might be performed in the current axis if its enabled on the configuration.
    - The cursor changes to reflect the situation.
    - If the user takes the opposite direction, the effect is immediate.

Cursor icons
------------
This tool uses a collection of cursor icons. See cursorIcon() for more details. The cursor is unset when the user releases the mouse button.

Implementation details
======================

There are three main blocks that share a common structure:
    - Scrolling.
    - Direction detection.
    - Cursor icon.

They make use of a start position and a current position which is modified when: 
    - __Scrolling__
        - When wrapping mouse around.
        - When detected direction changes.\n
          When doing scrolling can only happen to be:\n
            - From \link Direction::Undefined \endlink to \link 
              Direction::Vertical \endlink.
            - From \link Direction::Undefined \endlink to \link 
              Direction::Horizontal \endlink.
            - From \link Direction::Vertical \endlink to \link 
              Direction::Horizontal \endlink.
            - From \link Direction::Horizontal \endlink to \link 
              Direction::Vertical \endlink.
        - When a limit is reached.
        - When looping.
    - __Direction detection__
        - On mouse press.
        - When wrapping mouse around.
        - When direction detection transitions from undefined to vertical or 
          horizontal. \n
          This means every time DEFAULT_DETECTION_STEP_LENGTH is traveled.
    - __Cursor icon__
        - On mouse press.
        - When wrapping mouse around.
        - When calling cursorIcon().\n
          This means on every mouse move.

The start position is the point of reference from where the calculations are done (like an anchor), however those reference points may change to the current 
mouse position during the user interaction.

It is implemented this way in order to have sub-pixel accuracy and avoid rounding errors.

Scrolling
---------

When the start position is set, at the same time the current axis location is stored. The new location is found by with the following formula:

\code
newLocation = startLocation + distance(startPos, currentPos) / stepLength
\endcode

When a limit is reached:
 - The scroll is reset (beginScroll()) to provide an immediate effect when taking the opposite direction.
 - Scroll loop is applied when necessary.
 - Magnet effect ensures that exactly the last or first position is reached (in case an axis position is defined by a floating point number)

See scroll().

Mouse wraparound
----------------
When the mouse is outside the viewer, the operating system is told to teleport the cursor to the opposite direction. Expecting that the next mouse move event 
will be at the teleported location is not true because of events may be queued.

For this reason a mechanism is implemented to detect when the teleport event is actually received because the following tasks need to be performed:
    - Resetting the start position to the teleported location (beginScroll()).
    - Resetting the direction detection start position to the teleported location (beginDirectionDetection()).
    - Resetting the start position for setting the cursor icon to the teleported location (beginCursorIcon()).

See onMouseMove().

Direction detection
-------------------
The direction vector from the start point to the current position is calculated. If its length is greater than the DEFAULT_DETECTION_STEP_LENGTH the direction 
can be defined to be vertical or horizontal by looking at the greatest director vector component.

However, if there's already a defined direction (m_currentDirection) the required travel distance on the opposite component is doubled. Thus, if the user 
scrolls obliquely flip-flopping from vertical to horizontal axis is avoided.

See directionDetection().

Cursor icons
------------
Setting the cursor requires the following information:
    - Last start mouse position.
    - Current mouse position.
    - Current slice.
    - Minimum and maximum values. (For the limit reached icons)
    - Type of the current axis. (Phases or slices)

With that information any combination of these situations can be expressed:
    - Increment or decrement movement.
    - Upper or lower limit reached.
    - Scrolling of phases or slices.
    - Mouse direction (vertical, horizontal or undefined)

The method implementation calculates an index used to choose an icon from a hard-coded list.

In order to avoid futile updates, the last index of the set cursor set is stored. The cursor is updated only when the index changes.

The directionality of the cursor is determined by the direction vector defined by the start mouse position and the current mouse position. When this vector is 
(0,0) the cursor is simply not modified.

See cursorIcon().

*/
class SlicingMouseTool : public SlicingTool {
Q_OBJECT
public:
    explicit SlicingMouseTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingMouseTool();
    
    /** \brief Triggers move, press and release events.
     * 
     * \warning The release event happens whenever the right or left mouse buttons are released in order overcome the situation where the patient menu opening blocks receiving the release event of the left button.
     */
    virtual void handleEvent(unsigned long eventID) override;
    
public slots:
    /// \brief Assigns the axes modes for the current volume.
    virtual void reassignAxes() override;

protected:
    /// \brief Direction of the mouse movement.
    enum class Direction {Undefined, Vertical, Horizontal};
   
private:
    /// \brief Reads settings and sets the m_config_* members.
    void readConfiguration();
    
    ///\name Event handling
    //@{
    /// \brief Begins the movement, performs initializations and configurations.
    void onMousePress(const QPoint &position);
    /** \brief Reacting to the events when the dragging is active.
     * 
     * For more details read the class documentation.
     */
    void onMouseMove(const QPoint &position);
    /// \brief Unsets the cursor and finishes the movement.
    void onMouseRelease(const QPoint &position);
    //@}
    
    /** \brief Sets the cursor icon using the available information.
     * 
     * See also the class documentation for more details.
     * 
     * \pre Having set an start position with beginCursorIcon(). When the movement is started or the starting point modified, beginCursorIcon() shall be called.
     * \param position Current mouse position.
     * \sa beginCursorIcon()
     * \sa unsetCursorIcon()
    */
    void cursorIcon(const QPoint &currentPosition);
    /// \brief Changes the start position.
    void beginCursorIcon(const QPoint &startPosition);
    /// \brief Unsets the cursor icon and updates the index of the last cursor.
    void unsetCursorIcon();
    
    /** \brief Scrolls the current axis.
     * 
     * See also the class documentation for more details.
     * 
     * \pre Having a started the scrolling with beginScroll().
     * \pre Not having an \link Direction::Undefined \endlink direction (m_currentDirection).
     * \return The amount of unused scroll distance for the current axis.
     */
    double scroll(const QPoint& currentPosition);
    /** \brief Resets the current scroll and begins a new one.
     * 
     * Start position and current axis location are saved for posterior use.
     * 
     * The step distance in pixels required to scroll an slice is calulated here taking in account the viewer's size.
     * 
     * \param startPosition Start mouse position.
     */
    void beginScroll(const QPoint& startPosition);
    
    /** \brief Detects the direction from the current position to the start point.
     * 
     * See also the class documentation for more details.
     * 
     * \pre Having an start position with beginDirectionDetection().
     * \param currentPosition Current mouse position.
     * \return The detected direction or \link Direction::Undefined \endlink if it cannot be determined.
     */
    Direction directionDetection(const QPoint& currentPosition) const;
    /// \brief Sets the start mouse position from where the direction will be detected.
    void beginDirectionDetection(const QPoint& startPosition);
    
    /// \name Settings
    //@{
    bool m_config_sliceScrollLoop = false;
    bool m_config_phaseScrollLoop  = false;
    bool m_config_wraparound = false;
    //@}
    
    /// \brief Last mouse or start position. Used to determine the scroll direction of the icon.
    QPoint m_cursorIcon_lastPosition = QPoint(0,0);
    /// \brief Last cursor icon index used, this variable is used to avoid futile cursor updates.
    int m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
    /// \brief Default value to avoid a cursor icon change.
    static constexpr int CURSOR_ICON_DONT_UPDATE = -1;
    
    bool m_dragActive = false;
    
    /** \name Mouse wraparound
     * Variables used to detect the event resulting of having changed the cursor position.
     */
    //@{
    bool m_wraparound_wrappedToLeft = false;
    bool m_wraparound_wrappedToRight = false;
    bool m_wraparound_wrappedToTop = false;
    bool m_wraparound_wrappedToBottom = false;
    QPoint m_wraparound_positionBeforeWrapping = QPoint(0,0);
    //@}
    
    /// \name Current scroll status
    //@{
    double m_stepLength = 0;
    QPoint m_startPosition = QPoint(0,0);
    double m_startLocation = 0;
    bool m_scrollLoop = false;
    
    double m_directionDetection_stepLength = 0;
    QPoint m_directionDetection_startPosition = QPoint(0,0);
    
    Direction m_currentDirection = Direction::Undefined;
    //@}
    
    /// \name Distances in pixels
    //@{
    static constexpr unsigned int DEFAULT_MINIMUM_STEP_LENGTH = 2;
    static constexpr unsigned int DEFAULT_MAXIMUM_STEP_LENGTH = 64;
    static constexpr unsigned int DEFAULT_DETECTION_STEP_LENGTH = 16;
    //@}
    
    static constexpr unsigned int VERTICAL_AXIS = 0;
    static constexpr unsigned int HORIZONTAL_AXIS = 1;
    
};

}

#endif //UDGSLICINGMOUSETOOL_H
