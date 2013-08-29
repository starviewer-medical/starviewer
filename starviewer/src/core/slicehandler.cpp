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
    m_maxSliceValue = 0;
    m_minSliceValue = 0;
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

    m_numberOfPhases = m_volume->getNumberOfPhases();

    reset();
}

void SliceHandler::setViewPlane(const OrthogonalPlane &viewPlane)
{
    m_viewPlane = viewPlane;

    if (m_volume)
    {
        // Update the slice range for the new view
        m_volume->getSliceRange(m_minSliceValue, m_maxSliceValue, viewPlane);
    }
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

void SliceHandler::reset()
{
    setSlabThickness(1);
    setSlice(0);
    setPhase(0);
}

void SliceHandler::computeRangeAndSlice(int newSlabThickness)
{
    // First check the new value
    if (!hasSlabThicknessValueToBeUpated(newSlabThickness))
    {
        return;
    }
    
    if (newSlabThickness == 1)
    {
        m_slabThickness = 1;
        return;
    }

    int difference = newSlabThickness - m_slabThickness;
    // We distribute equally half of the difference of the new thickness above and below
    m_currentSlice -= difference / 2;
    
    // If difference is odd, we should then increase/decrease +-1 by one of its bounds (upper or lower)
    if (MathTools::isOdd(difference))
    {
        if (MathTools::isEven(m_slabThickness))
        {
            // If current thickness is even
            if (difference > 0)
            {
                // Decrease on lower bound when difference is positive
                m_currentSlice--;
                
                if (m_currentSlice < getMinimumSlice())
                {
                    // If we surpass lower bound, keep it in its corresponding bounds
                    m_currentSlice = getMinimumSlice();
                }
            }
        }
        else
        {
            // Otherwise (current thickness is odd)
            if (difference > 0)
            {
                // Upper bound will be increased when difference is positive
                int lastSlabSlice = getLastSlabSlice() + (difference / 2) + 1;
                
                if (lastSlabSlice > m_maxSliceValue)
                {
                    // If upper bound is surpassed, must decrease lower bound
                    m_currentSlice = m_maxSliceValue - newSlabThickness + 1;
                }
            }
            else
            {
                // Increase lower bound when difference is negative
                m_currentSlice++;
            }
        }
    }
    
    // Update thickness
    m_slabThickness = newSlabThickness;
}

bool SliceHandler::hasSlabThicknessValueToBeUpated(int thickness)
{
    if (thickness < 1)
    {
        DEBUG_LOG("Invalid thickness value. Must be >= 1.");
        return false;
    }
    
    if (thickness == m_slabThickness)
    {
        DEBUG_LOG("Same slab thickness, nothing is done.");
        return false;
    }
    
    if (thickness > m_maxSliceValue + 1)
    {
        DEBUG_LOG("New thickness exceeds maximum permitted thickness, it remains the same.");
        return false;
    }

    return true;
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
}

} // End namespace udg
