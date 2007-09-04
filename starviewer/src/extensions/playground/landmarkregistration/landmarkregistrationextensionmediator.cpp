/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "landmarkregistrationextensionmediator.h"

#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg {

LandmarkRegistrationExtensionMediator::LandmarkRegistrationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

LandmarkRegistrationExtensionMediator::~LandmarkRegistrationExtensionMediator()
{
}

DisplayableID LandmarkRegistrationExtensionMediator::getExtensionID() const
{
    return DisplayableID("LandmarkRegistrationExtension",tr("Landmark Registration"));
}

bool LandmarkRegistrationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QLandmarkRegistrationExtension *landmarkRegistrationExtension;

    if ( !(landmarkRegistrationExtension = qobject_cast<QLandmarkRegistrationExtension*>(extension)) )
    {
        return false;
    }

    landmarkRegistrationExtension->setInput( extensionContext.getDefaultVolume() );
    // TODO per evitar segmentation faults.
    landmarkRegistrationExtension->setSecondInput( extensionContext.getDefaultVolume() );

    return true;
}

}
