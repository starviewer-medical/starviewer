/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "registrationframeworkextensionmediator.h"

#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg {

RegistrationFrameworkExtensionMediator::RegistrationFrameworkExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

RegistrationFrameworkExtensionMediator::~RegistrationFrameworkExtensionMediator()
{
}

DisplayableID RegistrationFrameworkExtensionMediator::getExtensionID() const
{
    return DisplayableID("RegistrationFrameworkExtension",tr("Registration Framework"));
}

bool RegistrationFrameworkExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QRegistrationFrameworkExtension *registrationFrameworkExtension;

    if ( !(registrationFrameworkExtension = qobject_cast<QRegistrationFrameworkExtension*>(extension)) )
    {
        return false;
    }

    registrationFrameworkExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

}
