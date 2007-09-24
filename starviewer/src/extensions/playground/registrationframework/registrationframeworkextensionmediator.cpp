/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "registrationframeworkextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

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

bool RegistrationFrameworkExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QRegistrationFrameworkExtension *registrationFrameworkExtension;

    if ( !(registrationFrameworkExtension = qobject_cast<QRegistrationFrameworkExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        registrationFrameworkExtension->setInput( input );

    return true;
}

}
