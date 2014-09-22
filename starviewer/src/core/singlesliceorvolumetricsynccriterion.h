#ifndef SINGLESLICEORVOLUMETRICSYNCCRITERION_H
#define SINGLESLICEORVOLUMETRICSYNCCRITERION_H

#include "synccriterion.h"

namespace udg {

/**
    Implements a SyncCriterion that checks that both viewers contain either a single slice or a true volume.
 */
class SingleSliceOrVolumetricSyncCriterion : public SyncCriterion {
public:
    SingleSliceOrVolumetricSyncCriterion();
    virtual ~SingleSliceOrVolumetricSyncCriterion();

protected:
    virtual bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

};

} // End namespace udg

#endif
