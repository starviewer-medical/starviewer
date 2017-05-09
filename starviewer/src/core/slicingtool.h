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

#ifndef UDGSLICINGTOOL_H
#define UDGSLICINGTOOL_H

#include "tool.h"

// Qt
#include <QVector>

namespace udg {

class Q2DViewer;

/**
Base class for slicing tools composed of axes which can be assigned an scrolling mode.

Use this class as a base class to implement your slicing tools. It will abstract your specific implementation to deal with the viewer.

The axis concept is like an slider UI control:
 - Has a current position.
 - Position can be changed within an specified range.
 - The range has a minimum and maximum value.

An SlicingTool can have as many axes as you want. Use setNumberOfAxes(unsigned int numberOfAxes) to set them.

Each axis can be assigned a mode (\link SlicingTool::SlicingMode \endlink). A mode is something that can be scrolled like:
    - \link SlicingMode::None \endlink \n
      Does nothing (default value).
    - \link SlicingMode::Slice \endlink \n
      Slices of the current viewer.
    - \link SlicingMode::Phase \endlink \n
      Phases of the current viewer.
    - \link SlicingMode::Volume \endlink \n
      Volumes of the current study.

When implementing your own slicing tool, you should model its behaviour with axes using those methods:
    - getMinimum(unsigned int axis) const
    - getMaximum(unsigned int axis) const
    - getRangeSize(unsigned int axis) const
    - getLocation(unsigned int axis) const
    - setLocation(unsigned int axis, double location)
    - incrementLocation(unsigned int axis, double shift)

However if you need to direct interaction without dealing with axes, you may use those other methods:
    - getMinimum(SlicingMode mode) const
    - getMaximum(SlicingMode mode) const
    - getRangeSize(SlicingMode mode) const
    - getLocation(SlicingMode mode) const
    - setLocation(SlicingMode mode, double location)
    - incrementLocation(SlicingMode mode, double shift)

When a volume is loaded or there's another major change to the viewer like a change on thick slab, etc... you can react to it by implementing the reassignAxes
function. Then you can decide which function assign on each axis depending on the context (for example, in a series that has only phases you might want to 
scroll phases on booth axes).

Axes mode assignations are stored in a vector where the axis number is the vector index. The number of axes and its modes can be changed on the fly with:
    - setNumberOfAxes(unsigned int numberOfAxes)
    - setMode(unsigned int axis, SlicingMode mode)

Albeit volumes and phases will always be an integer, your implementation should be prepared to handle decimal positions. For this reason incrementLocation() 
returns a decimal number which is the amount of unused increment. For example, when setting to the phase 0.6 will mean rounding to 1 and thus having an excess 
of -0.4. Your implementation may use this information to know when a limit has been reached.

Note that when rounding occurs, the unused amounts will be in the -0.5 to +0.5 interval, however when a limit is reached, that interval will be escaped.
 */
class SlicingTool : public Tool {
    Q_OBJECT
public:
    /// \brief Modes an axis may take
    enum class SlicingMode { 
        None,  /**< A default mode that scrolls nothing. */
        Slice, /**< Scrolls slices rounding to the nearest integer position. */
        Phase, /**< Scrolls phases rounding to the nearest integer position. */
        Volume /**< Scrolls volumes rounding to the nearest integer position. */
    };

    /// \brief Sets the number of axes to zero.
    explicit SlicingTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingTool();
    
    /// \name Without axes
    //@{
    
    /** \brief Minimum location for the specified mode.
     * \return May be negative and always 0 with \link SlicingMode::None \endlink.
     */
    
    double getMinimum(SlicingMode mode) const;
    /** \brief Maximum location for the specified mode.
     * \return May be negative and always -1 with \link SlicingMode::None \endlink.
     */
    
    double getMaximum(SlicingMode mode) const;
    /** \brief Distance or number of slices between the minimum and the maximum.
     * \return Always greater or equal than 0.
     */
    double getRangeSize(SlicingMode mode) const;
    
    /** \brief Current location for the specified mode.
     * 
     * The location will always be within the minimum and maximum bounds.
     * 
     * \return May be negative and always 0 with \link SlicingMode::None \endlink.
     */
    double getLocation(SlicingMode mode) const;
    
    /** \brief Set the specified mode to an absolute location.
     * 
     * Depending on the mode the set location may differ from the desired \p location specified.
     *     - If the mode requires an integer position, the location will be rounded.
     *     - If the location is less than the minimum, the location will be set to the minimum.
     *     - If the location is greater than the minimum, the location will be set to the maximum.
     * 
     * If the mode is \link SlicingMode::None \endlink nothing happens.
     * 
     * \param location The desired location to set.
     * \return The real location that has been set.
     */
    double setLocation(SlicingMode mode, double location);
    
    /** \brief Increment the current mode location. 
     * 
     * Gets the current location and sets a new location with the specified increment (\p shift).
     * 
     * \code
     * unusedShift = desiredDestinationLocation - realDestinationLocation
     * \endcode
     * 
     * Note the following cases regarding the returned value.
     *     - 0 means that all the shift could be applied.
     *     - [-0.5, 0.5] rounding to the nearest slice.
     *     - Less than -0.5 the lower limit has been reached.
     *     - Greater than +0.5 the upper limit has been reached.
     * 
     * \sa setLocation(SlicingMode mode, double location)
     * \param shift Desired increment to apply. Negative numbers are allowed.
     * \return The real increment that has been applied.
     */
    double incrementLocation(SlicingMode mode, double shift);
    //@}
    
    /// \brief Number of axes.
    unsigned int getNumberOfAxes() const;
    /** \brief Change the number of available axes.
     * 
     * If the number of axes grows, new axes are set to \link SlicingMode::None \endlink, the others remain untouched.
     */
    void setNumberOfAxes(unsigned int numberOfAxes);
    
    /** \brief Get the mode from an axis number.
     * \return The axis mode or \link SlicingMode::None \endlink if the axis does not exist.
     */
    SlicingMode getMode(unsigned int axis) const;
    /** \brief Tell an axis to have a mode.
     * 
     * If the axis index does not exist, nothing happens.
     */
    void setMode(unsigned int axis, SlicingMode mode);
    
    /// \name With axes abstraction
    //@{
    /** \brief Calls getMinimum(SlicingMode mode) const with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double getMinimum(unsigned int axis) const;
    /** \brief Calls getMaximum(SlicingMode mode) const with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double getMaximum(unsigned int axis) const;
    /** \brief Calls getRangeSize(SlicingMode mode) const with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double getRangeSize(unsigned int axis) const;
    /** \brief Calls getLocation(SlicingMode mode) const with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double getLocation(unsigned int axis) const;
    /** \brief Calls setLocation(SlicingMode mode, double location) with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double setLocation(unsigned int axis, double location);
    /** \brief Calls incrementLocation(SlicingMode mode, double shift) with the mode of the axis.
     * 
     * If the axis does not exist, the mode is considered to be \link SlicingMode::None \endlink.
     */
    double incrementLocation(unsigned int axis, double shift);
    //@}
    
public slots:
    /// \brief Reimplement to react to major changes like having a new volume, changing thich slab thickness, view plane, etc...
    virtual void reassignAxes() = 0;
    
protected:
    /// \brief Showing the minimum or maximum when a new volume is loaded.
    bool m_volumeInitialPositionToMaximum = false;
    Q2DViewer* m_2DViewer;

private:
    QVector<SlicingMode> m_axes;
};

}

#endif //UDGSLICINGTOOL_H
