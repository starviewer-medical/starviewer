#ifndef UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H
#define UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

/// Includes needed to do the factory register
#include "zoomfactorsyncaction.h"
#include "signaltosyncactionmapperfactoryregister.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map zoom factor changes to ZoomFactorSyncAction
 */
class ZoomFactorSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    ZoomFactorSignalToSyncActionMapper(QObject *parent = 0);
    ~ZoomFactorSignalToSyncActionMapper();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given factor to a ZoomFactorSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(double factor);
};

/// With this line we register this mapper on the factory
static SignalToSyncActionMapperFactoryRegister<ZoomFactorSignalToSyncActionMapper> registerZoomFactorSignalToSyncActionMapper(ZoomFactorSyncAction().getMetaData());

} // End namespace udg

#endif

