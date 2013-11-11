#ifndef THICKSLABSYNCACTION_H
#define THICKSLABSYNCACTION_H

#include "syncaction.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for thick slab propagation.
 */
class ThickSlabSyncAction : public SyncAction {

public:
    ThickSlabSyncAction();
    virtual ~ThickSlabSyncAction();

    /// Sets the slab projection mode to be propagated.
    void setSlabProjectionMode(int slabProjectionMode);

    /// Sets the slab thickess to be propagated.
    void setSlabThickness(int numberOfSlices);

    /// Sets the volume to be thick-slab-synced across viewers.
    void setVolume(Volume *volume);

    /// Propagates the stored thick slab properties to the stored volume in the given viewer.
    virtual void run(QViewer *viewer);

protected:
    /// Sets up the metadata of this sync action.
    virtual void setupMetaData();

    /// Sets up the default list of SyncCriterion.
    virtual void setupDefaultSyncCriteria();

private:
    /// Slab projection mode to be propagated.
    int m_slabProjectionMode;

    /// Slab thickness to be propagated.
    int m_slabThickness;

    /// Volume to be thick-slab-synced across viewers.
    Volume *m_volume;

};

}

#endif
