#ifndef PHASESYNCACTION_H
#define PHASESYNCACTION_H

#include "syncaction.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for phase propagation.
 */
class PhaseSyncAction : public SyncAction {

public:
    PhaseSyncAction();
    virtual ~PhaseSyncAction();

    /// Sets the phase index to be propagated.
    void setPhase(int phase);

    /// Sets the volume to be phase-synced across viewers.
    void setVolume(Volume *volume);

    /// Propagates the stored phase index to the stored volume in the given viewer.
    virtual void run(QViewer *viewer);

protected:
    /// Sets up the metadata of this sync action.
    virtual void setupMetaData();

    /// Sets up the default list of SyncCriterion.
    virtual void setupDefaultSyncCriteria();

private:
    /// Phase index to be propagated.
    int m_phase;

    /// Volume to be phase-synced across viewers.
    Volume *m_volume;

};

}

#endif
