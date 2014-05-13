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

    void setSlice(int slice);
    int getCurrentSlice() const;

    int getMinimumSlice() const;

    /// Returns the maximum slice that could be set, so it takes into account current slice thickness
    int getMaximumSlice() const;
    
    /// Returns the total number of slices on the spatial dimension for the current view plane
    int getNumberOfSlices() const;
    
    void setPhase(int phase);
    int getCurrentPhase() const;

    int getNumberOfPhases() const;
    
    void setSlabThickness(int thickness);
    int getSlabThickness() const;

    int getLastSlabSlice() const;

    /// Returns slice thickness of the currently displayed image.
    /// On the acquisition plane, this depends on DICOM's slice thickness and slab thickness (if DICOM's slice thickness is not defined, the method returns 0).
    /// On the other planes, this depends on the spacing and the slab thickness.
    double getSliceThickness() const;

protected:
    virtual bool isLoopEnabledForSlices() const;
    virtual bool isLoopEnabledForPhases() const;

protected:
    int m_minSliceValue;
    int m_numberOfSlices;
    int m_numberOfPhases;

private:
    /// Called when setting a new Volume to reset slab thickness, slice and phase.
    void reset();

    /// Returns true if the given slab thickness value is valid and false otherwise.
    bool isValidSlabThickness(int thickness);

private:
    Volume *m_volume;
    OrthogonalPlane m_viewPlane;
    int m_slabThickness;
    int m_currentSlice;
    int m_currentPhase;

};

} // End namespace udg

#endif
