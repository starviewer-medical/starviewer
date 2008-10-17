#include "experimental3dextensionmediator.h"

#include <QMessageBox>

#include "extensioncontext.h"


namespace udg {


Experimental3DExtensionMediator::Experimental3DExtensionMediator( QObject *parent )
 : ExtensionMediator( parent )
{
}


Experimental3DExtensionMediator::~Experimental3DExtensionMediator()
{
}


DisplayableID Experimental3DExtensionMediator::getExtensionID() const
{
    return DisplayableID( "Experimental3DExtension", tr("Experimental 3D") );
}


bool Experimental3DExtensionMediator::initializeExtension( QWidget *extension, const ExtensionContext &extensionContext )
{
    QExperimental3DExtension *experimental3DExtension;

    if ( !( experimental3DExtension = qobject_cast<QExperimental3DExtension*>( extension ) ) ) return false;

    Volume *input = extensionContext.getDefaultVolume();

    if( !input ) QMessageBox::information( 0, tr("Starviewer"), tr("The selected item is not an image") );
    else experimental3DExtension->setInput( input );

    return true;
}


}
