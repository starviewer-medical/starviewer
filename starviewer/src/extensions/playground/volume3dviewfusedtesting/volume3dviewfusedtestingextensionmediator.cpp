/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volume3dviewfusedtestingextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

Volume3DViewFusedTestingExtensionMediator::Volume3DViewFusedTestingExtensionMediator( QObject * parent )
    : ExtensionMediator( parent )
{
}

Volume3DViewFusedTestingExtensionMediator::~Volume3DViewFusedTestingExtensionMediator()
{
}

DisplayableID Volume3DViewFusedTestingExtensionMediator::getExtensionID() const
{
    return DisplayableID( "Volume3DViewFusedTestingExtension", tr("Fused 3D Viewer" ));
}

bool Volume3DViewFusedTestingExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QVolume3DViewFusedTestingExtension * volume3DViewFusedTestingExtension;

    if ( !( volume3DViewFusedTestingExtension = qobject_cast< QVolume3DViewFusedTestingExtension * >( extension ) ) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
	    volume3DViewFusedTestingExtension->setInput( input );

    return true;
}



}
