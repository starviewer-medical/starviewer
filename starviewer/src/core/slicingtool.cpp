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

#include "slicingtool.h"

#include "mathtools.h"
#include "q2dviewer.h"
#include "volume.h"
#include "patient.h"
#include "changesliceqviewercommand.h"
#include "changephaseqviewercommand.h"

namespace udg {

SlicingTool::SlicingTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_volumeInitialPositionToMaximum(false)
{
    m_toolName = "SlicingTool";
    
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    // Must have a valid 2DViewer
    Q_ASSERT(m_2DViewer);
    
    setNumberOfAxes(0);
    
    // The number of axes might change on volume changed.
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reassignAxes()));
    connect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(reassignAxes()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(reassignAxes())); // Sagittal, axial, coronal, swivel orientation change...
}

SlicingTool::~SlicingTool()
{
}

double SlicingTool::getMinimum(SlicingTool::SlicingMode mode) const
{
    if (mode == SlicingMode::Slice)
    { // Slices can be negative
        return m_2DViewer->getMinimumSlice();
    }
    return 0;
}

double SlicingTool::getMaximum(SlicingTool::SlicingMode mode) const
{
    if (mode == SlicingMode::Slice)
    {
        return m_2DViewer->getMaximumSlice();
    }
    else if (mode == SlicingMode::Phase)
    {
        return m_2DViewer->getNumberOfPhases() - 1;
    }
    else if (mode == SlicingMode::Volume)
    {
        //NOTE: Evaluation lazyness used to check null pointers before they are used.
        if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient()) 
        {
            return m_2DViewer->getMainInput()->getPatient()->getNumberOfVolumes() - 1;
        }
    }
    return -1;
}

double SlicingTool::getRangeSize(SlicingTool::SlicingMode mode) const
{
    return getMaximum(mode) - getMinimum(mode) + 1;
}

double SlicingTool::getLocation(SlicingMode mode) const
{
    if (mode == SlicingMode::Slice)
    {
        return m_2DViewer->getCurrentSlice();
    }
    else if (mode == SlicingMode::Phase)
    {
        return m_2DViewer->getCurrentPhase();
    }
    else if (mode == SlicingMode::Volume)
    {
        //NOTE: Evaluation lazyness used to check null pointers before they are used.
        if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient()) 
        {
            QList<Volume*> volumes = m_2DViewer->getMainInput()->getPatient()->getVolumesList();
            int volumeIndex = volumes.indexOf(m_2DViewer->getMainInput());
            Q_ASSERT(volumeIndex >= 0); // Volume must be found in the list. //BUG: Be careful if dummyvolume is loaded; you should control this externally.
            return volumeIndex;
        }
    }
    return 0;
}

double SlicingTool::setLocation(SlicingMode mode, double location)
{
    const double min = getMinimum(mode);
    const double max = getMaximum(mode);
    double newLocation;
    
    if (location <= min + MathTools::Epsilon)
    { 
        // Underflow, or very close albeit not exceeding the lower bound.
        newLocation = min;
    }
    else if (location >= max - MathTools::Epsilon)
    { 
        // Overflow, or very close albeit not exceeding the upper bound.
        newLocation = max;
    }
    else
    { 
        // Within the limits
        newLocation = location;
    }

    if (min <= max)
    {
        // Interval width greater than zero. Changing position feasible.
        if (mode == SlicingMode::Slice)
        {
            newLocation = std::round(newLocation);
            m_2DViewer->setSlice(newLocation);
        }
        else if (mode == SlicingMode::Phase)
        {
            newLocation = std::round(newLocation);
            m_2DViewer->setPhase(newLocation);
        }
        else if (mode == SlicingMode::Volume)
        {
            newLocation = std::round(newLocation);
            //NOTE: Evaluation lazyness used to check null pointers before they are used.
            if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient()) 
            {
                QList<Volume*> volumes = m_2DViewer->getMainInput()->getPatient()->getVolumesList();
                Volume* nextVolume = volumes.at(newLocation); // Volume must be found in the list.
                QViewerCommand* command;
                if (nextVolume->getNumberOfSlicesPerPhase() > 1)
                {
                    // Multiple slices per phase
                    command = new ChangeSliceQViewerCommand(m_2DViewer, m_volumeInitialPositionToMaximum ? ChangeSliceQViewerCommand::SlicePosition::MaximumSlice : ChangeSliceQViewerCommand::SlicePosition::MinimumSlice);
                }
                else
                {
                    // One slice per phase
                    command = new ChangePhaseQViewerCommand(m_2DViewer, m_volumeInitialPositionToMaximum ? ChangePhaseQViewerCommand::PhasePosition::MaximumPhase : ChangePhaseQViewerCommand::PhasePosition::MinimumPhase);
                }
                m_2DViewer->setInputAsynchronously(nextVolume, command);
            }
        }
    }
    return newLocation;
}

double SlicingTool::incrementLocation(SlicingMode mode, double shift)
{
    double location = getLocation(mode) + shift;
    double newLocation = setLocation(mode, location);
    return  location - newLocation;
}

unsigned int SlicingTool::getNumberOfAxes() const
{
    return m_axes.size();
}

void SlicingTool::setNumberOfAxes(unsigned int numberOfAxes)
{
    m_axes.fill(SlicingMode::None, numberOfAxes);   
}

SlicingTool::SlicingMode SlicingTool::getMode(unsigned int axis) const
{
    return getNumberOfAxes() > axis ? m_axes[axis] : SlicingMode::None;
}

void SlicingTool::setMode(unsigned int axis, SlicingMode mode)
{
    if (getNumberOfAxes() > axis)
    {
        m_axes[axis] = mode;
    }
}

double SlicingTool::getMinimum(unsigned int axis) const
{
    return getMinimum(getMode(axis));
}

double SlicingTool::getMaximum(unsigned int axis) const
{
    return getMaximum(getMode(axis));
}

double SlicingTool::getRangeSize(unsigned int axis) const
{
    return getRangeSize(getMode(axis));
}

double SlicingTool::getLocation(unsigned int axis) const
{
    return getLocation(getMode(axis));
}

double SlicingTool::setLocation(unsigned int axis, double location)
{
    return setLocation(getMode(axis), location);
}

double SlicingTool::incrementLocation(unsigned int axis, double shift)
{
    return incrementLocation(getMode(axis), shift);
}

}
