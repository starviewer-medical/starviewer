#include "dicomprintextensionmediator.h"

#include "extensioncontext.h"
#include "series.h"
#include "volume.h"
#include "image.h"

// QT
#include <QMessageBox>

namespace udg {

DicomPrintExtensionMediator::DicomPrintExtensionMediator(QObject *parent): ExtensionMediator(parent)
{
}

DicomPrintExtensionMediator::~DicomPrintExtensionMediator()
{
}

DisplayableID DicomPrintExtensionMediator::getExtensionID() const
{
    return DisplayableID("DicomPrintExtension",tr("Dicom Print"));
}

bool DicomPrintExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{   
	QDicomPrintExtension *dicomPrintExtension;

	if ( !(dicomPrintExtension = qobject_cast<QDicomPrintExtension*>(extension)) )
    {
        return false;
    } 

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        dicomPrintExtension->setInput( input );

    return true;
}
}