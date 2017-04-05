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

#include "q2dviewer.h"
#include "volume.h"
#include "patient.h"
#include "changesliceqviewercommand.h"
#include "changephaseqviewercommand.h"

namespace udg {


SlicingTool::SlicingTool(QViewer *viewer, QObject *parent) : 
Tool(viewer, parent)
{
    m_volumeInitialPositionToMaximum = false;
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    setNumberOfAxes(0);
    
    m_toolName = "SlicingTool";
    
    // Must have a valid 2DViewer
    Q_ASSERT(m_2DViewer);
    
    // The number of axes might change on volume changed.
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reassignAxis()));
    connect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(reassignAxis()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(reassignAxis())); // Sagittal, axial, coronal, swivel orientation change...
}

SlicingTool::~SlicingTool()
{
}

int SlicingTool::getMinimum(SlicingTool::SlicingMode mode) const
{
    if (mode == SlicingMode::Slice)
    { // Slices can be negative
        return m_2DViewer->getMinimumSlice();
    }
    return 0;
}

int SlicingTool::getMaximum(SlicingTool::SlicingMode mode) const
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

unsigned int SlicingTool::getRangeSize(SlicingTool::SlicingMode mode) const
{
    return getMaximum(mode) - getMinimum(mode) + 1;
}

int SlicingTool::getLocation(SlicingMode mode) const
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

int SlicingTool::setLocation(SlicingMode mode, int location, bool dryRun)
{
    int min = getMinimum(mode);
    int max = getMaximum(mode);
    int overflow = location <= max ? 0 : location - max;
    int underflow = location >= min ? 0 : location - min;
    // Examples
    // loc	min	max	uflow	oflow
    // -2	 0	 5	-2	 0
    //  0	 0	 5	 0	 0
    //  5	 0	 5	 0	 0
    //  7	 0	 5	 0	 2
    // 
    //  3	 5	 10	-2	 0
    //  5	 5	 10	 0	 0
    //  10	 5	 10	 0	 0
    //  12	 5	 10	 0	 2
    // 
    // -7	-5	 5	-2	 0
    // -5	-5	 5	 0	 0
    //  5	-5	 5	 0	 0
    //  7	-5	 5	 0	 2
    // 
    // -7	-5	 0	-2	 0
    // -5	-5	 0	 0	 0
    //  0	-5	 0	 0	 0
    //  2	-5	 0	 0	 2
    // 
    // -12	-10	-5	-2	 0
    // -10	-10	-5	 0	 0
    // -5	-10	-5	 0	 0
    // -3	-10	-5	 0	 2
    // Zero width interval:
    //  0	 0	-1	 0	 1
    // -1	 0	-1	-1	 0
    // Zero width interval:
    //  4	 5	 4	-1	 0
    //  5	 5	 4	 0	 1
    
    int returnValue = 0;
    if (overflow != 0)
    {
        location = max;
        returnValue = overflow;
    }
    else if (underflow != 0)
    {
        location = min;
        returnValue = underflow;
    }
    
    if (!dryRun)
    {
        if (min <= max) // Interval width greater than zero. Changing position feasible.
        {
            if (mode == SlicingMode::Slice)
            {
                m_2DViewer->setSlice(location);
            }
            else if (mode == SlicingMode::Phase)
            {
                m_2DViewer->setPhase(location);
                
            }
            else if (mode == SlicingMode::Volume)
            {
                //NOTE: Evaluation lazyness used to check null pointers before they are used.
                if (m_2DViewer->getMainInput() && m_2DViewer->getMainInput()->getPatient()) 
                {
                    QList<Volume*> volumes = m_2DViewer->getMainInput()->getPatient()->getVolumesList();
                    Volume* nextVolume = volumes.at(location); // Volume must be found in the list.
                    QViewerCommand* command;
                    if (nextVolume->getNumberOfSlicesPerPhase() > 1)
                    { // Multiple slices per phase
                        command = new ChangeSliceQViewerCommand(m_2DViewer, m_volumeInitialPositionToMaximum ? ChangeSliceQViewerCommand::SlicePosition::MaximumSlice : ChangeSliceQViewerCommand::SlicePosition::MinimumSlice);
                    }
                    else
                    { // One slice per phase
                        command = new ChangePhaseQViewerCommand(m_2DViewer, m_volumeInitialPositionToMaximum ? ChangePhaseQViewerCommand::PhasePosition::MaximumPhase : ChangePhaseQViewerCommand::PhasePosition::MinimumPhase);
                    }
                    m_2DViewer->setInputAsynchronously(nextVolume, command);
                }
            }
        }
    }
    return returnValue;
}

int SlicingTool::incrementLocation(SlicingMode mode, int shift, bool dryRun)
{
    return setLocation(mode, getLocation(mode) + shift, dryRun);
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

int SlicingTool::getMinimum(unsigned int axis) const
{
    return getMinimum(getMode(axis));
}

int SlicingTool::getMaximum(unsigned int axis) const
{
    return getMaximum(getMode(axis));
}

unsigned int SlicingTool::getRangeSize(unsigned int axis) const
{
    return getRangeSize(getMode(axis));
}

int SlicingTool::getLocation(unsigned int axis) const
{
    return getLocation(getMode(axis));
}

int SlicingTool::setLocation(unsigned int axis, int location, bool dryRun)
{
    return setLocation(getMode(axis), location, dryRun);
}

int SlicingTool::incrementLocation(unsigned int axis, int shift, bool dryRun)
{
    return incrementLocation(getMode(axis), shift, dryRun);
}


}
