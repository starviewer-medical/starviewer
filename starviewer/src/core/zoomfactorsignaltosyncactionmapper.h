#ifndef UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H
#define UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map zoom factor changes to ZoomFactorSyncAction
 */
class ZoomFactorSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    ZoomFactorSignalToSyncActionMapper(QObject *parent = 0);
    ~ZoomFactorSignalToSyncActionMapper();

    /// Maps current zoom factor of the viewer
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given factor to a ZoomFactorSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(double factor);
};

} // End namespace udg

#endif

