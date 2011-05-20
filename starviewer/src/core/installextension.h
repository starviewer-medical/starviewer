#ifndef UDGINSTALLEXTENSION_H
#define UDGINSTALLEXTENSION_H

#include "extensionfactoryregister.h"
#include "extensionmediatorfactoryregister.h"

namespace udg {

/**
    Exemple:
    @code
    InstallExtension<3DMPRExtension, 3DMPRExtensionMediator> registerMe;
    @endcode
  */
template <class Extension, class Mediator>
class InstallExtension {
public:
    InstallExtension()
    {
        Mediator m;
        ExtensionFactoryRegister<Extension> registerFactory(m.getExtensionID().getID());
        ExtensionMediatorFactoryRegister<Mediator> registerMediator(m.getExtensionID().getID());
    }
};

}

#endif
