#include "gputestingextensionmediator.h"

#include <QMessageBox>

#include "extensioncontext.h"


namespace udg {


GpuTestingExtensionMediator::GpuTestingExtensionMediator( QObject *parent )
 : ExtensionMediator( parent )
{
}


GpuTestingExtensionMediator::~GpuTestingExtensionMediator()
{
}


DisplayableID GpuTestingExtensionMediator::getExtensionID() const
{
    return DisplayableID( "GpuTestingExtension", tr("GPU testing") );
}


bool GpuTestingExtensionMediator::initializeExtension( QWidget *extension, const ExtensionContext &extensionContext )
{
    QGpuTestingExtension *gpuTestingExtension;

    if ( !( gpuTestingExtension = qobject_cast<QGpuTestingExtension*>( extension ) ) ) return false;

    Volume *input = extensionContext.getDefaultVolume();

    if( !input ) QMessageBox::information( 0, tr("Starviewer"), tr("The selected item is not an image") );
    else gpuTestingExtension->setInput( input );

    return true;
}


}
