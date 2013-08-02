#ifndef UDGSIGNALTOSYNCACTIONMAPPERFACTORYREGISTER_H
#define UDGSIGNALTOSYNCACTIONMAPPERFACTORYREGISTER_H

#include "genericsingletonfactoryregister.h"
#include "signaltosyncactionmapperfactory.h"

#include "logging.h"

namespace udg {

/**
    This class enables the registration of a derived instace of SignalToSyncActionMapper into SignalToSyncActionMapperFactory.
    In order to register SignalToSyncActionMapper we should declare an object of SignalToSyncActionMapperFactoryRegister type.
    Example:
    @code
    SignalToSyncActionMapperFactoryRegister<SignalToSyncActionMapperDerivedClassName> registerAs(SyncActionMetaData("IdentifierName", tr("User Interface name"), "settingName"));
    @endcode
    This simple line of code is enough to register a derived instace of SignalToSyncActionMapper into SignalToSyncActionMapperFactory.
  */
template <class FactoryType>
class SignalToSyncActionMapperFactoryRegister : 
    public GenericSingletonFactoryRegister<SignalToSyncActionMapper, FactoryType, SyncActionMetaData, SignalToSyncActionMapperFactory, QObject> {

public:
    SignalToSyncActionMapperFactoryRegister(const SyncActionMetaData &identifier)
     : GenericSingletonFactoryRegister<SignalToSyncActionMapper, FactoryType, SyncActionMetaData, SignalToSyncActionMapperFactory, QObject>(identifier)
    {
        DEBUG_LOG("SignalToSyncActionMapperFactoryRegister" + identifier.getName());
    }
};

} // End namespace udg

#endif
