/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREGISTRATIONFRAMEWORKEXTENSIONMEDIATOR_H
#define UDGREGISTRATIONFRAMEWORKEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qregistrationframeworkextension.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RegistrationFrameworkExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    RegistrationFrameworkExtensionMediator(QObject *parent = 0);

    ~RegistrationFrameworkExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QRegistrationFrameworkExtension, RegistrationFrameworkExtensionMediator> registerRegistrationFrameworkExtension;

}

#endif
