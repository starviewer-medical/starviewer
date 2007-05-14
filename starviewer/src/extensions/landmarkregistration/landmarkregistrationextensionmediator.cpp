/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "landmarkregistrationextensionmediator.h"

#include "volumerepository.h"
#include "extensionhandler.h"

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

bool LandmarkRegistrationExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    QLandmarkRegistrationExtension *landmarkRegistrationExtension;

    if ( !(landmarkRegistrationExtension = qobject_cast<QLandmarkRegistrationExtension*>(extension)) )
    {
        return false;
    }

    connect( landmarkRegistrationExtension, SIGNAL( newSerie() ), extensionHandler, SLOT( openSerieToCompare() ) );
    connect( extensionHandler , SIGNAL( secondInput(Volume*) ) , landmarkRegistrationExtension , SLOT( setSecondInput(Volume*) ) );

    landmarkRegistrationExtension->setInput(VolumeRepository::getRepository()->getVolume( mainVolumeID ));

    return true;
}

}
