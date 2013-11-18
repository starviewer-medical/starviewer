#ifndef UDGWINDOWLEVELSIGNALTOSYNCACTIONMAPPER_H
#define UDGWINDOWLEVELSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

/// Includes needed to do the factory register
#include "windowlevelsyncaction.h"
#include "signaltosyncactionmapperfactoryregister.h"

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

/// With this line we register this mapper on the factory
static SignalToSyncActionMapperFactoryRegister<WindowLevelSignalToSyncActionMapper> registerWindowLevelSignalToSyncActionMapper(WindowLevelSyncAction().getMetaData());

} // End namespace udg

#endif
