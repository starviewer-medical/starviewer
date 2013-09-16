#ifndef UDGWINDOWLEVELSYNCACTION_H
#define UDGWINDOWLEVELSYNCACTION_H

#include "syncaction.h"
#include "windowlevel.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for a window level
    The set window level on setWindowLevel() will be applied when run() is called
 */
class WindowLevelSyncAction : public SyncAction {
public:
    WindowLevelSyncAction();
    ~WindowLevelSyncAction();

    /// Sets the window level to be synched
    void setWindowLevel(const WindowLevel &windowLevel);
    void setVolume(Volume *volume);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Window level that will be applied on run()
    WindowLevel m_windowLevel;
    Volume *m_volume;
};

} // End namespace udg

#endif
