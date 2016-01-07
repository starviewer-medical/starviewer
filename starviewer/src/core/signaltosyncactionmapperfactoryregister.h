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
