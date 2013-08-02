#ifndef UDZOOMFACTORSYNCACTION_H
#define UDZOOMFACTORSYNCACTION_H

#include "syncaction.h"

namespace udg {

/**
    Implementation of a SyncAction for a zoom factor
    The set zoom factor on setZoomFactor() will be applied when run() is called
 */
class ZoomFactorSyncAction : public SyncAction {
public:
    ZoomFactorSyncAction();
    ~ZoomFactorSyncAction();

    /// Sets zoom factor to be synched
    void setZoomFactor(double factor);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Zoom factor that will be applied on run()
    double m_zoomFactor;
};

} // End namespace udg

#endif

