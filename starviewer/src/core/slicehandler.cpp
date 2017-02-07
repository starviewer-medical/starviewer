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

namespace {

// Returns the normal that corresponds to the given orthogonal plane.
Vector3 getNormalFromOrthogonalPlane(const OrthogonalPlane &orthogonalPlane)
{
    switch (orthogonalPlane)
    {
        case OrthogonalPlane::XYPlane: return Vector3(0, 0, 1);
        case OrthogonalPlane::XZPlane: return Vector3(0, 1, 0);
        case OrthogonalPlane::YZPlane: return Vector3(1, 0, 0);
        case OrthogonalPlane::None: throw std::invalid_argument("Normal not defined for OrthogonalPlane::None");
    }
}

// Returns the orthogonal plane that corresponds to the given plane.
OrthogonalPlane getOrthogonalPlaneFromPlane(const Plane &plane)
{
    if (plane.getNormal() == Vector3(0, 0, 1))
    {
        return OrthogonalPlane::XYPlane;
    }
    else if (plane.getNormal() == Vector3(0, 1, 0))
    {
        return OrthogonalPlane::XZPlane;
    }
    else if (plane.getNormal() == Vector3(1, 0, 0))
    {
        return OrthogonalPlane::YZPlane;
    }
    else
    {
        return OrthogonalPlane::None;
    }
}

}

SliceHandler::SliceHandler()
    : m_volume(nullptr), m_referenceViewPlane(Vector3(0, 0 ,1), Vector3(0, 0, 0)), m_position(0), m_minPosition(0), m_maxPosition(0), m_stepDistance(1),
      m_phase(0), m_numberOfPhases(1), m_slabThickness(0), m_snapToSlice(true)
{
}

SliceHandler::~SliceHandler()
{
}

void SliceHandler::setVolume(Volume *volume)
{
    m_volume = volume;

    this->setOrthogonalViewPlane(OrthogonalPlane::XYPlane);

    m_numberOfPhases = m_volume ? m_volume->getNumberOfPhases() : 1;

    setSlabThickness(0.0);
    setSlice(0);
    setPhase(0);
}

const Plane& SliceHandler::getReferenceViewPlane() const
{
    return m_referenceViewPlane;
}

void SliceHandler::setReferenceViewPlane(Plane viewPlane)
{
    m_referenceViewPlane = std::move(viewPlane);
    m_orthogonalViewPlane = getOrthogonalPlaneFromPlane(m_referenceViewPlane);

    if (m_volume)
    {
        auto corners = m_volume->getCorners();
        m_minPosition = std::numeric_limits<double>::infinity();
        m_maxPosition = -std::numeric_limits<double>::infinity();

        for (const Vector3 &corner : corners)
        {
            double distance = m_referenceViewPlane.signedDistanceToPoint(corner);
            m_minPosition = std::min(distance, m_minPosition);
            m_maxPosition = std::max(distance, m_maxPosition);
        }
    }
}

const OrthogonalPlane& SliceHandler::getOrthogonalViewPlane() const
{
    return m_orthogonalViewPlane;
}

void SliceHandler::setOrthogonalViewPlane(const OrthogonalPlane &viewPlane)
{
    Vector3 normal = getNormalFromOrthogonalPlane(viewPlane);
    Vector3 origin;
    if (m_volume)
    {
        origin = m_volume->getOrigin();
        setStepDistance(m_volume->getSpacing()[viewPlane.getZIndex()]);
    }
    setReferenceViewPlane(Plane(normal, origin));
}

double SliceHandler::getPosition() const
{
    return m_position;
}

void SliceHandler::setPosition(double position)
{
    m_position = qBound(getMinimumPosition(), position, getMaximumPosition());

    if (m_snapToSlice)
    {
        setSlice(getSlice());
    }
}

double SliceHandler::getMinimumPosition() const
{
    return m_minPosition + m_slabThickness / 2;
}

double SliceHandler::getMaximumPosition() const
{
    return m_maxPosition - m_slabThickness / 2;
}

double SliceHandler::getDefaultStepDistance() const
{
    if (m_volume)
    {
        auto spacing = m_volume->getSpacing();

        if (this->getOrthogonalViewPlane() == OrthogonalPlane::None)
        {
            // Compute a transformed z-spacing like vtkImageReslice does
            const auto &normal = m_referenceViewPlane.getNormal();
            return normal.x * normal.x * spacing[0] + normal.y * normal.y * spacing[1] + normal.z * normal.z * spacing[2];
        }
        else
        {
            return spacing[this->getOrthogonalViewPlane().getZIndex()];
        }
    }
    else
    {
        return 1;
    }
}

double SliceHandler::getStepDistance() const
{
    return m_stepDistance;
}

void SliceHandler::setStepDistance(double stepDistance)
{
    m_stepDistance = stepDistance;
}

int SliceHandler::getSlice() const
{
    return qRound(m_position / m_stepDistance);
}

void SliceHandler::setSlice(int slice)
{
    slice = MathTools::getBoundedValue(slice, getMinimumSlice(), getMaximumSlice(), isLoopEnabledForSlices());
    m_position = slice * m_stepDistance;
}

int SliceHandler::getMinimumSlice() const
{
    return qRound(getMinimumPosition() / m_stepDistance);
}

int SliceHandler::getMaximumSlice() const
{
    return qRound(getMaximumPosition() / m_stepDistance);
}

int SliceHandler::getNumberOfSlices() const
{
    int minSliceValue = qRound(m_minPosition / m_stepDistance);
    int maxSliceValue = qRound(m_maxPosition / m_stepDistance);
    return maxSliceValue - minSliceValue + 1;
}

int SliceHandler::getPhase() const
{
    return m_phase;
}

void SliceHandler::setPhase(int phase)
{
    m_phase = MathTools::getBoundedValue(phase, 0, m_numberOfPhases - 1, isLoopEnabledForPhases());
}

int SliceHandler::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

double SliceHandler::getSlabThickness() const
{
    return m_slabThickness;
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
    if (m_snapToSlice)
    {
        int slice = qBound(getMinimumSlice(), getSlice(), getMaximumSlice());
        setSlice(slice);
    }
    else
    {
        double position = qBound(getMinimumPosition(), getPosition(), getMaximumPosition());
        setPosition(position);
    }
}

double SliceHandler::getMaximumSlabThickness() const
{
    return m_maxPosition - m_minPosition;
}

bool SliceHandler::getSnapToSlice() const
{
    return m_snapToSlice;
}

void SliceHandler::setSnapToSlice(bool snapToSlice)
{
    m_snapToSlice = snapToSlice;
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
            Image *image = getImage();

            if (image)
            {
                thickness = image->getSliceThickness();
            }
            else
            {
                // Compute a transformed thickness like vtkImageReslice does
                const auto &normal = m_referenceViewPlane.getNormal();
                std::array<double, 3> voxelSize;
                m_volume->getSpacing(voxelSize.data());
                if ((image = m_volume->getImage(0, getPhase())))
                {
                    voxelSize[2] = image->getSliceThickness();
                }
                thickness = normal.x * normal.x * voxelSize[0] + normal.y * normal.y * voxelSize[1] + normal.z * normal.z * voxelSize[2];
            }
        }

        return thickness;
    }
}

Image* SliceHandler::getImage() const
{
    constexpr double Tolerance = 1e-6;

    Image *image = nullptr;

    if (m_volume && this->getOrthogonalViewPlane() == OrthogonalPlane::XYPlane)
    {
        int nearestSlice = qRound(m_position / getDefaultStepDistance());
        double nearestImagePosition = nearestSlice * getDefaultStepDistance();

        if (std::abs(m_position - nearestImagePosition) < Tolerance)
        {
            image = m_volume->getImage(nearestSlice, getPhase());
        }
    }

    return image;
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

bool SliceHandler::isValidSlabThickness(double thickness)
{
    return thickness >= 0.0;
}

} // End namespace udg
