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
    m_slabThickness = 0.0;
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
    return m_minSliceValue + getNumberOfSlicesInSlabThickness() / 2;
}

int SliceHandler::getMaximumSlice() const
{
    return m_minSliceValue + m_numberOfSlices - 1 - getNumberOfSlicesInSlabThickness() / 2;
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

void SliceHandler::setSlabThickness(double thickness)
{
    // If the new thickness is invalid or is the same as the current one, then do nothing
    if (!isValidSlabThickness(thickness) || thickness == m_slabThickness)
    {
        return;
    }

    // Keep thickness in bounds
    if (thickness > getMaximumSlabThickness())
    {
        thickness = getMaximumSlabThickness();
        DEBUG_LOG("New thickness exceeds maximum permitted thickness, setting it to maximum.");
    }

    // Set the thickness (m_slabThickness)
    m_slabThickness = thickness;

    // Keep the slab in range
    m_currentSlice = qBound(getMinimumSlice(), m_currentSlice, getMaximumSlice());
}

double SliceHandler::getSlabThickness() const
{
    return m_slabThickness;
}

double SliceHandler::getMaximumSlabThickness() const
{
    if (m_volume)
    {
        int zIndex = this->getViewPlane().getZIndex();
        return this->getNumberOfSlices() * m_volume->getSpacing()[zIndex];
    }
    else
    {
        return 0.0;
    }
}

int SliceHandler::getNumberOfSlicesInSlabThickness() const
{
    if (m_volume)
    {
        int zIndex = this->getViewPlane().getZIndex();
        return qRound(m_slabThickness / m_volume->getSpacing()[zIndex]);
    }
    else
    {
        return 0;
    }
}

double SliceHandler::getSliceThickness() const
{
    if (m_slabThickness > 0.0)
    {
        return m_slabThickness;
    }
    else
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
                        }
                    }
                    break;

                case OrthogonalPlane::YZPlane:
                    thickness = m_volume->getSpacing()[0];
                    break;

                case OrthogonalPlane::XZPlane:
                    thickness = m_volume->getSpacing()[1];
                    break;
            }
        }

        return thickness;
    }
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
    setSlabThickness(0.0);
    setSlice(0);
    setPhase(0);
}

bool SliceHandler::isValidSlabThickness(double thickness)
{
    return thickness >= 0.0;
}

} // End namespace udg
