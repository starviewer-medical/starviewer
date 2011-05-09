#include "q3dviewerextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

Q3DViewerExtensionMediator::Q3DViewerExtensionMediator(QObject *parent)
    : ExtensionMediator(parent)
{
}

Q3DViewerExtensionMediator::~Q3DViewerExtensionMediator()
{
}

DisplayableID Q3DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q3DViewerExtension", tr("3D Viewer"));
}

bool Q3DViewerExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    Q3DViewerExtension *q3DViewerExtension;

    if (!(q3DViewerExtension = qobject_cast<Q3DViewerExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if (!input)
    {
        QMessageBox::information(0, tr("Starviewer"), tr("The selected item is not an image"));
    }
    else
    {
        q3DViewerExtension->setInput(input);
    }

    return true;
}

}
