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

void SliceHandler::setSlice(int slice)
{
    if (m_currentSlice != slice)
    {
        updateSlice(slice);
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
    updateSlab(thickness);
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

void SliceHandler::updateSlab(int newSlabThickness)
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

    int thicknessDifference = newSlabThickness - m_slabThickness;
    // We distribute equally half of the difference of the new thickness above and below
    m_currentSlice -= thicknessDifference / 2;
    
    if (MathTools::isOdd(thicknessDifference))
    {
        // If thickness difference is odd, we should then increase/decrease the extra slice left of thickness
        // on upper or lower bound depending on some conditions
        
        if (MathTools::isEven(m_slabThickness))
        {
            if (thicknessDifference > 0)
            {
                // Decrease on lower bound when thickness has been increased and keep it inside bounds
                m_currentSlice = qMax(m_currentSlice - 1, getMinimumSlice());
            }
        }
        else
        {
            if (thicknessDifference > 0)
            {
                // When thickness has been increased and current thickness is odd, upper bound will be increased
                // and thus we must check if it will be out of upper bounds to update the lower bound accordingly
                int lastSlabSlice = getLastSlabSlice() + (thicknessDifference / 2) + 1;
                
                if (lastSlabSlice > m_maxSliceValue)
                {
                    // If upper bound is surpassed, must decrease lower bound
                    m_currentSlice = m_maxSliceValue - newSlabThickness + 1;
                }
            }
            else
            {
                // Increase lower bound when thickness has been decreased (when current thickness is odd)
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

void SliceHandler::updateSlice(int slice)
{
    m_currentSlice = MathTools::getBoundedValue(slice, 0, m_maxSliceValue - m_slabThickness + 1, isLoopEnabledForSlices());
}

} // End namespace udg
