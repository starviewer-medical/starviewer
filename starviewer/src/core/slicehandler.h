#ifndef UDGSLICEHANDLER_H
#define UDGSLICEHANDLER_H

#include <QObject>

#include "orthogonalplane.h"

namespace udg {

class Volume;

/**
    Handles the proper computing of slices, phases, slab thickness and the related ranges
 */
class SliceHandler : public QObject {
Q_OBJECT
public:
    SliceHandler(QObject *parent = 0);
    ~SliceHandler();

    void setVolume(Volume *volume);

    void setViewPlane(const OrthogonalPlane &viewPlane);
    const OrthogonalPlane& getViewPlane() const;

    void setSlice(int value);
    int getCurrentSlice() const;

    int getMinimumSlice() const;
    int getMaximumSlice() const;
    
    void setPhase(int value);
    int getCurrentPhase() const;

    int getNumberOfPhases() const;
    
    void setSlabThickness(int thickness);
    int getSlabThickness() const;

    int getLastSlabSlice() const;

protected:
    virtual bool isLoopEnabledForSlices() const;
    virtual bool isLoopEnabledForPhases() const;

protected:
    int m_minSliceValue;
    int m_maxSliceValue;
    int m_numberOfPhases;

private:
    void computeRangeAndSlice(int newSlabThickness);
    
    void checkAndUpdateSliceValue(int value);

private:
    Volume *m_volume;
    OrthogonalPlane m_viewPlane;
    int m_slabThickness;
    int m_currentSlice;
    int m_currentPhase;
    int m_lastSlabSlice;

};

} // End namespace udg

#endif
