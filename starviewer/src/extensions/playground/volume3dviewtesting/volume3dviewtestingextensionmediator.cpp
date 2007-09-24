/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volume3dviewtestingextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

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

bool Volume3DViewTestingExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QVolume3DViewTestingExtension * volume3DViewTestingExtension;

    if ( !( volume3DViewTestingExtension = qobject_cast< QVolume3DViewTestingExtension * >( extension ) ) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        volume3DViewTestingExtension->setInput( input );

    return true;
}



}
