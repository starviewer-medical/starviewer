#include "slicehandler.h"

#include "coresettings.h"
#include "image.h"
#include "logging.h"
#include "volume.h"

namespace udg {

SliceHandler::SliceHandler(QObject *parent)
 : QObject(parent), m_volume(0)
{
    m_currentSlice = 0;
    m_maxSliceValue = 0;
    m_minSliceValue = 0;
    m_currentPhase = 0;
    m_numberOfPhases = 1;
    m_slabThickness = 1;
    m_lastSlabSlice = 0;
}

SliceHandler::~SliceHandler()
{
}

void SliceHandler::setVolume(Volume *volume)
{
    m_volume = volume;

    this->setSlice(0);
    this->setPhase(0);

    this->setViewPlane(OrthogonalPlane::XYPlane);

    m_numberOfPhases = m_volume->getNumberOfPhases();
}

void SliceHandler::setViewPlane(const OrthogonalPlane &viewPlane)
{
    m_viewPlane = viewPlane;

    // Update the slice range for the new view
    m_volume->getSliceRange(m_minSliceValue, m_maxSliceValue, viewPlane);
}

const OrthogonalPlane& SliceHandler::getViewPlane() const
{
    return m_viewPlane;
}

void SliceHandler::setSlice(int value)
{
    if (m_currentSlice != value)
    {
        checkAndUpdateSliceValue(value);
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
    return m_maxSliceValue;
}

void SliceHandler::setPhase(int value)
{
    if (m_currentPhase != value)
    {
        if (isLoopEnabledForPhases())
        {
            if (value < 0)
            {
                value = m_numberOfPhases - 1;
            }
            else if (value > m_numberOfPhases - 1)
            {
                value = 0;
            }
        }
        else
        {
            if (value < 0)
            {
                value = 0;
            }
            else if (value > m_numberOfPhases - 1)
            {
                value = m_numberOfPhases - 1;
            }
        }

        m_currentPhase = value;
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
    computeRangeAndSlice(thickness);
    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;
}

int SliceHandler::getSlabThickness() const
{
    return m_slabThickness;
}

int SliceHandler::getLastSlabSlice() const
{
    return m_lastSlabSlice;
}

double SliceHandler::getSliceThickness() const
{
    double thickness = 0.0;

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

void SliceHandler::computeRangeAndSlice(int newSlabThickness)
{
    // First check the new value
    if (newSlabThickness < 1)
    {
        DEBUG_LOG("Invalid thickness value. Must be >= 1.");
        return;
    }
    if (newSlabThickness == m_slabThickness)
    {
        DEBUG_LOG("Same slab thickness, nothing is done.");
        return;
    }
    if (newSlabThickness > m_maxSliceValue + 1)
    {
        DEBUG_LOG("New thickness exceeds maximum permitted thickness, it remains the same.");
        return;
    }
    if (newSlabThickness == 1)
    {
        m_slabThickness = 1;
        return;
    }

    int difference = newSlabThickness - m_slabThickness;
    // If difference is positive, increase thickness
    if (difference > 0)
    {
        // Integer division
        m_currentSlice -= difference / 2;
        m_lastSlabSlice += difference / 2;

        // If difference is odd, increase +1 by one of its bounds (upper or lower)
        if ((difference % 2) != 0)
        {
            // If current thickness is pair, grow on lower bound
            if ((m_slabThickness % 2) == 0)
            {
                m_currentSlice--;
            }
            // Otherwise grow on upper bound
            else
            {
                m_lastSlabSlice++;
            }
        }
        // Check if we exceed min/max range
        if (m_currentSlice < getMinimumSlice())
        {
            // If exceeding on lower bound, must grow on upper bound
            m_lastSlabSlice = getMinimumSlice() + newSlabThickness - 1;
            m_currentSlice = getMinimumSlice();
        }
        else if (m_lastSlabSlice > m_maxSliceValue)
        {
            // If exceeding on upper bound, must grow on lower bound
            m_currentSlice = m_maxSliceValue - newSlabThickness + 1;
            m_lastSlabSlice = m_maxSliceValue;
        }
    }
    // Negative difference, decrease thickness
    else
    {
        // Convert difference to positive value for ease of computing
        difference *= -1;
        m_currentSlice += difference / 2;
        m_lastSlabSlice -= difference / 2;

        // If difference is odd, decrease +1 by one of its bounds (upper or lower)
        if ((difference % 2) != 0)
        {
            // If current thickness is pair, decrease on upper bound
            if ((m_slabThickness % 2) == 0)
            {
                m_lastSlabSlice--;
            }
            // Otherwise decrease on lower bound
            else
            {
                m_currentSlice++;
            }
        }
    }
    // Update thickness
    m_slabThickness = newSlabThickness;
}

void SliceHandler::checkAndUpdateSliceValue(int value)
{
    if (isLoopEnabledForSlices())
    {
        if (value < 0)
        {
            value = m_maxSliceValue - m_slabThickness + 1;
        }
        else if (value + m_slabThickness - 1 > m_maxSliceValue)
        {
            value = 0;
        }
    }
    else
    {
        if (value < 0)
        {
            value = 0;
        }
        else if (value + m_slabThickness - 1 > m_maxSliceValue)
        {
            value = m_maxSliceValue - m_slabThickness + 1;
        }
    }

    m_currentSlice = value;

    m_currentSlice = m_currentSlice;
    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;
}

} // End namespace udg
