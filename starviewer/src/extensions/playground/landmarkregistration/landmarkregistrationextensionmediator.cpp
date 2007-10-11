/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "landmarkregistrationextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

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

bool LandmarkRegistrationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QLandmarkRegistrationExtension *landmarkRegistrationExtension;

    if ( !(landmarkRegistrationExtension = qobject_cast<QLandmarkRegistrationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
    {
        landmarkRegistrationExtension->setInput( input );
        // TODO per evitar segmentation faults.
        landmarkRegistrationExtension->setSecondInput( input );
    }

    return true;
}

}
