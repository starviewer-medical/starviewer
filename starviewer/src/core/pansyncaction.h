#ifndef UDPANSYNCACTION_H
#define UDPANSYNCACTION_H

#include "syncaction.h"

namespace udg {

/**
    Implementation of a SyncAction for a pan action
    The set motion vector on setMotionVector() will be applied when run() is called
 */
class PanSyncAction : public SyncAction {
public:
    PanSyncAction();
    ~PanSyncAction();

    /// Sets motion vector to be synched
    void setMotionVector(double vector[3]);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Motion vector that will be applied on run()
    double m_motionVector[3];
};

} // End namespace udg

#endif
