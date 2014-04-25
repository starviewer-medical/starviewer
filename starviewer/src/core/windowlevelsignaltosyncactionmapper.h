#ifndef UDGWINDOWLEVELSIGNALTOSYNCACTIONMAPPER_H
#define UDGWINDOWLEVELSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

class WindowLevel;

/**
    Implementation of SignalToSyncActionMapper to map window level changes to WindowLevelSyncAction
 */
class WindowLevelSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    WindowLevelSignalToSyncActionMapper(QObject *parent = 0);
    ~WindowLevelSignalToSyncActionMapper();

    /// Maps current WW/WL of the viewer
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given WindowLevel to a WindowLevelSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(const WindowLevel &windowLevel);
};

} // End namespace udg

#endif
