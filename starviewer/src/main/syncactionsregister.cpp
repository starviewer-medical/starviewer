#include "syncactionsregister.h"

#include "signaltosyncactionmapperfactoryregister.h"

// For each *SignalToSyncActionMapper we want to be registered globally, we should add its include here
// and register it on registerSyncActions()
#include "windowlevelsignaltosyncactionmapper.h"
#include "zoomfactorsignaltosyncactionmapper.h"
#include "pansignaltosyncactionmapper.h"
#include "imageorientationsignaltosyncactionmapper.h"
#include "phasesignaltosyncactionmapper.h"
#include "thickslabsignaltosyncactionmapper.h"

// Include each sync action
#include "windowlevelsyncaction.h"
#include "zoomfactorsyncaction.h"
#include "pansyncaction.h"
#include "imageorientationsyncaction.h"
#include "phasesyncaction.h"
#include "thickslabsyncaction.h"

namespace udg {

void SyncActionsRegister::registerSyncActions()
{
    // With each line we register each mapper on the factory
    static SignalToSyncActionMapperFactoryRegister<WindowLevelSignalToSyncActionMapper> registerWindowLevelSignalToSyncActionMapper(WindowLevelSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ZoomFactorSignalToSyncActionMapper> registerZoomFactorSignalToSyncActionMapper(ZoomFactorSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<PanSignalToSyncActionMapper> registerPanSignalToSyncActionMapper(PanSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ImageOrientationSignalToSyncActionMapper> registerImageOrientationSignalToSyncActionMapper(ImageOrientationSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<PhaseSignalToSyncActionMapper> registerPhaseSignalToSyncActionMapper(PhaseSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ThickSlabSignalToSyncActionMapper> registerThickSlabSignalToSyncActionMapper(ThickSlabSyncAction().getMetaData());
}

}
