#include "imageprintextensionmediator.h"

#include "extensioncontext.h"
#include "series.h"
#include "volume.h"
#include "image.h"

// QT
#include <QMessageBox>

namespace udg {

ImagePrintExtensionMediator::ImagePrintExtensionMediator(QObject *parent): ExtensionMediator(parent)
{
}

ImagePrintExtensionMediator::~ImagePrintExtensionMediator()
{
}

DisplayableID ImagePrintExtensionMediator::getExtensionID() const
{
    return DisplayableID("ImagePrintExtension",tr("Image Print"));
}

bool ImagePrintExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{   
	QImagePrintExtension *imagePrintExtension;

	if ( !(imagePrintExtension = qobject_cast<QImagePrintExtension*>(extension)) )
    {
        return false;
    } 
    
    return true;
}
}