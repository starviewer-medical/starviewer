#ifndef UDGPANSIGNALTOSYNCACTIONMAPPER_H
#define UDGPANSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map motion vector changes to PanSyncAction
 */
class PanSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    PanSignalToSyncActionMapper(QObject *parent = 0);
    ~PanSignalToSyncActionMapper();

    /// Maps current pan factor of the viewer
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given factor to a PanSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(double *vector);
};

} // End namespace udg

#endif

