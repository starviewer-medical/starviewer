/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class Extension, class Mediator>
class InstallExtension
{
public:    
    InstallExtension()
    {
        Mediator m;
        ExtensionFactoryRegister<Extension> registerFactory( m.getExtensionID().getID() );
        ExtensionMediatorFactoryRegister<Mediator> registerMediator( m.getExtensionID().getID() );
    }
};

}

#endif
