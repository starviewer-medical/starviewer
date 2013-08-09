#ifndef UDGSLICEHANDLER_H
#define UDGSLICEHANDLER_H

#include <QObject>

namespace udg {

/**
    Handles the proper computing of slices, phases, slab thickness and the related ranges
 */
class SliceHandler : public QObject {
Q_OBJECT
public:
    SliceHandler(QObject *parent = 0);
    ~SliceHandler();

    void setSlice(int value);
    int getCurrentSlice() const;
    
    void setSliceRange(int min, int max);
    int getMinimumSlice() const;
    int getMaximumSlice() const;
    
    void setPhase(int value);
    int getCurrentPhase() const;

    void setNumberOfPhases(int value);
    int getNumberOfPhases() const;
    
    void setSlabThickness(int thickness);
    int getSlabThickness() const;

    int getLastSlabSlice() const;

protected:
    virtual bool isLoopEnabledForSlices() const;
    virtual bool isLoopEnabledForPhases() const;

private:
    void computeRangeAndSlice(int newSlabThickness);
    
    void checkAndUpdateSliceValue(int value);

private:
    int m_slabThickness;
    int m_currentSlice;
    int m_currentPhase;
    int m_numberOfPhases;
    int m_lastSlabSlice;
    int m_maxSliceValue;
    int m_minSliceValue;
};

} // End namespace udg

#endif
