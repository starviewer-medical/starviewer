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

#include "slicehandler.h"

#include "coresettings.h"
#include "image.h"
#include "mathtools.h"
#include "logging.h"
#include "volume.h"

namespace udg {

SliceHandler::SliceHandler(QObject *parent)
 : QObject(parent), m_volume(0)
{
    m_currentSlice = 0;
    m_minSliceValue = 0;
    m_numberOfSlices = 1;
    m_currentPhase = 0;
    m_numberOfPhases = 1;
    m_slabThickness = 1;
}

SliceHandler::~SliceHandler()
{
}

void SliceHandler::setVolume(Volume *volume)
{
    m_volume = volume;

    this->setViewPlane(OrthogonalPlane::XYPlane);

    if (m_volume)
    {
        m_numberOfPhases = m_volume->getNumberOfPhases();
    }

    reset();
}

void SliceHandler::setViewPlane(const OrthogonalPlane &viewPlane)
{
    m_viewPlane = viewPlane;

    if (m_volume)
    {
        // Update the slice range for the new view
        int maxSliceValue;
        m_volume->getSliceRange(m_minSliceValue, maxSliceValue, viewPlane);
        m_numberOfSlices = maxSliceValue - m_minSliceValue + 1;
    }
}

const OrthogonalPlane& SliceHandler::getViewPlane() const
{
    return m_viewPlane;
}

void SliceHandler::setSlice(int slice)
{
    if (m_currentSlice != slice)
    {
        m_currentSlice = MathTools::getBoundedValue(slice, getMinimumSlice(), getMaximumSlice(), isLoopEnabledForSlices());
    }
}

int SliceHandler::getCurrentSlice() const
{
    return m_currentSlice;
}

int SliceHandler::getMinimumSlice() const
{
    return m_minSliceValue;
}

int SliceHandler::getMaximumSlice() const
{
    return m_minSliceValue + m_numberOfSlices - m_slabThickness;
}

int SliceHandler::getNumberOfSlices() const
{
    return m_numberOfSlices;
}

void SliceHandler::setPhase(int phase)
{
    if (m_currentPhase != phase)
    {
        m_currentPhase = MathTools::getBoundedValue(phase, 0, m_numberOfPhases - 1, isLoopEnabledForPhases());
    }
}

int SliceHandler::getCurrentPhase() const
{
    return m_currentPhase;
}

int SliceHandler::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

void SliceHandler::setSlabThickness(int thickness)
{
    // If the new thickness is invalid or is the same as the current one, then do nothing
    if (!isValidSlabThickness(thickness) || thickness == m_slabThickness)
    {
        return;
    }

    // Keep thickness in bounds
    if (thickness > m_numberOfSlices)
    {
        thickness = m_numberOfSlices;
        DEBUG_LOG("New thickness exceeds maximum permitted thickness, setting it to maximum.");
    }

    // The slices of the slab are distributed with the following criterion:
    // (in the examples 'X' represents the original slice and '|' represents the additional slab slices)
    // - Assume that we start with a thickness of 1
    // - Slices are added alternately on each side, starting towards the end, e.g.: X, X|, |X|, |X||, ||X||, ...
    // - Slices are removed in the inverse order, e.g.: ..., ||X||, |X||, |X|, X|, X
    // - If the slab range is out of range, it is moved appropriately to the beginning or the end

    // This behaviour is implemented with this algorithm:
    // 1. Find the slice at the center of the current slab; if the current thickness is even, choose the left center slice
    int centerSlice = m_currentSlice + (m_slabThickness - 1) / 2;

    // 2. Set the first slice (m_currentSlice) so that the first (maybe smaller) half of the additional (not counting the center) slices are before the center;
    //    the other (maybe bigger) half goes after the center
    m_currentSlice = centerSlice - (thickness - 1) / 2;

    // 3. Set the thickness (m_slabThickness)
    m_slabThickness = thickness;

    // 4. Keep the slab in range (getMaximumSlice() takes m_slabThickness into account)
    m_currentSlice = qBound(getMinimumSlice(), m_currentSlice, getMaximumSlice());
}

int SliceHandler::getSlabThickness() const
{
    return m_slabThickness;
}

int SliceHandler::getLastSlabSlice() const
{
    return m_currentSlice + m_slabThickness - 1;
}

double SliceHandler::getSliceThickness() const
{
    double thickness = 0.0;

    if (m_volume)
    {
        switch (this->getViewPlane())
        {
            case OrthogonalPlane::XYPlane:
                {
                    Image *image = m_volume->getImage(this->getCurrentSlice(), this->getCurrentPhase());

                    if (image)
                    {
                        thickness = image->getSliceThickness();

                        if (this->getSlabThickness() > 1)
                        {
                            double gap = m_volume->getSpacing()[2] - thickness;

                            if (gap < 0)
                            {
                                // If gap between spacing and thickness is negative, this means slices overlap, so
                                // we have to substract this gap between to get the real thickness
                                thickness = (thickness + gap) * this->getSlabThickness();
                            }
                            else
                            {
                                thickness = thickness * this->getSlabThickness();
                            }
                        }
                    }
                }
                break;

            case OrthogonalPlane::YZPlane:
                thickness = m_volume->getSpacing()[0] * this->getSlabThickness();
                break;

            case OrthogonalPlane::XZPlane:
                thickness = m_volume->getSpacing()[1] * this->getSlabThickness();
                break;
        }
    }

    return thickness;
}

bool SliceHandler::isLoopEnabledForSlices() const
{
    Settings settings;
    return settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool();
}

bool SliceHandler::isLoopEnabledForPhases() const
{
    Settings settings;
    return settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool();
}

void SliceHandler::reset()
{
    setSlabThickness(1);
    setSlice(0);
    setPhase(0);
}

bool SliceHandler::isValidSlabThickness(int thickness)
{
    if (thickness < 1)
    {
        DEBUG_LOG("Invalid thickness value. Must be >= 1.");
        return false;
    }

    return true;
}

} // End namespace udg
