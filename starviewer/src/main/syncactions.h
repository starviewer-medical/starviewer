#ifndef SYNCACTIONS_H
#define SYNCACTIONS_H

// Here, by including each *SignalToSyncActionMapper, we register them on the factory
// So, for each *SignalToSyncActionMapper we want to be registered globally, we should add its include here

#include "windowlevelsignaltosyncactionmapper.h"
#include "zoomfactorsignaltosyncactionmapper.h"
#include "pansignaltosyncactionmapper.h"
#include "imageorientationsignaltosyncactionmapper.h"

#endif