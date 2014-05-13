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
