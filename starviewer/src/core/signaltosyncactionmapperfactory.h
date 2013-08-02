#ifndef UDGSIGNALTOSYNCACTIONMAPPERFACTORY_H
#define UDGSIGNALTOSYNCACTIONMAPPERFACTORY_H

#include "genericfactory.h"
#include "singleton.h"
#include "signaltosyncactionmapper.h"
#include "syncactionmetadata.h"

namespace udg {

/**
    \fn typedef Singleton<GenericFactory<SignalToSyncActionMapper, SyncActionMetaData> > SignalToSyncActionMapperFactory
    Typedef for the class used to create a derived instance of SignalToSyncActionMapper on run time.
    Intended for internal use to register a SignalToSyncActionMapper instance.
  */
typedef Singleton<GenericFactory<SignalToSyncActionMapper, SyncActionMetaData> > SignalToSyncActionMapperFactory;

}

#endif
