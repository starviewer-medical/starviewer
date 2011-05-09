#include "dicomprintextensionmediator.h"

#include "extensioncontext.h"
#include "series.h"
#include "volume.h"
#include "image.h"
#include "starviewerapplication.h"

// QT
#include <QMessageBox>

namespace udg {

DicomPrintExtensionMediator::DicomPrintExtensionMediator(QObject *parent) : ExtensionMediator(parent)
{
}

DicomPrintExtensionMediator::~DicomPrintExtensionMediator()
{
}

DisplayableID DicomPrintExtensionMediator::getExtensionID() const
{
    return DisplayableID("DicomPrintExtension", tr("DICOM Print"));
}

bool DicomPrintExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    QDicomPrintExtension *dicomPrintExtension;

    if (!(dicomPrintExtension = qobject_cast<QDicomPrintExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if (!input)
    {
        QMessageBox::information(0, udg::ApplicationNameString, tr("The selected item is not an image"));
    }
    else
    {
        dicomPrintExtension->setInput(input);
    }

    return true;
}
}
