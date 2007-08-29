/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "volume3dviewtestingextensionmediator.h"
#include "extensioncontext.h"

namespace udg {

Volume3DViewTestingExtensionMediator::Volume3DViewTestingExtensionMediator( QObject * parent )
    : ExtensionMediator( parent )
{
}

Volume3DViewTestingExtensionMediator::~Volume3DViewTestingExtensionMediator()
{
}

DisplayableID Volume3DViewTestingExtensionMediator::getExtensionID() const
{
    return DisplayableID( "Volume3DViewTestingExtension", tr("Volume 3D View Testing") );
}

bool Volume3DViewTestingExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    QVolume3DViewTestingExtension * volume3DViewTestingExtension;

    if ( !( volume3DViewTestingExtension = qobject_cast< QVolume3DViewTestingExtension * >( extension ) ) )
    {
        return false;
    }

    volume3DViewTestingExtension->setInput( extensionContext.getDefaultVolume() );

    return true;
}



}
