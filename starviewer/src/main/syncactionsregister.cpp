/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "syncactionsregister.h"

#include "signaltosyncactionmapperfactoryregister.h"

// For each *SignalToSyncActionMapper we want to be registered globally, we should add its include here
// and register it on registerSyncActions()
#include "voilutsignaltosyncactionmapper.h"
#include "zoomfactorsignaltosyncactionmapper.h"
#include "pansignaltosyncactionmapper.h"
#include "imageorientationsignaltosyncactionmapper.h"
#include "phasesignaltosyncactionmapper.h"
#include "thickslabsignaltosyncactionmapper.h"

// Include each sync action
#include "voilutsyncaction.h"
#include "zoomfactorsyncaction.h"
#include "pansyncaction.h"
#include "imageorientationsyncaction.h"
#include "phasesyncaction.h"
#include "thickslabsyncaction.h"

namespace udg {

void SyncActionsRegister::registerSyncActions()
{
    // With each line we register each mapper on the factory
    static SignalToSyncActionMapperFactoryRegister<VoiLutSignalToSyncActionMapper> registerVoiLutSignalToSyncActionMapper(VoiLutSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ZoomFactorSignalToSyncActionMapper> registerZoomFactorSignalToSyncActionMapper(ZoomFactorSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<PanSignalToSyncActionMapper> registerPanSignalToSyncActionMapper(PanSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ImageOrientationSignalToSyncActionMapper> registerImageOrientationSignalToSyncActionMapper(ImageOrientationSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<PhaseSignalToSyncActionMapper> registerPhaseSignalToSyncActionMapper(PhaseSyncAction().getMetaData());
    static SignalToSyncActionMapperFactoryRegister<ThickSlabSignalToSyncActionMapper> registerThickSlabSignalToSyncActionMapper(ThickSlabSyncAction().getMetaData());
}

}
