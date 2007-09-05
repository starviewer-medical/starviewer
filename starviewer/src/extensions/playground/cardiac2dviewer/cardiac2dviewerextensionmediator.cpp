/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cardiac2dviewerextensionmediator.h"

#include "extensioncontext.h"

namespace udg {

Cardiac2DViewerExtensionMediator::Cardiac2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Cardiac2DViewerExtensionMediator::~Cardiac2DViewerExtensionMediator()
{
}

DisplayableID Cardiac2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Cardiac2dViewerExtension",tr("2D Cardiac Viewer"));
}

bool Cardiac2DViewerExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QCardiac2DViewerExtension *cardiac2DViewerExtension;

    if ( !(cardiac2DViewerExtension = qobject_cast<QCardiac2DViewerExtension*>(extension)) )
    {
        return false;
    }

    cardiac2DViewerExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}

}
