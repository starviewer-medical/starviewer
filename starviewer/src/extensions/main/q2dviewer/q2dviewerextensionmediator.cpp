#include "q2dviewerextensionmediator.h"

#include "extensioncontext.h"
#include "starviewerapplication.h"
// QT
#include <QMessageBox>

namespace udg {

Q2DViewerExtensionMediator::Q2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Q2DViewerExtensionMediator::~Q2DViewerExtensionMediator()
{
}

DisplayableID Q2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q2DViewerExtension", tr("2D Viewer"));
}

bool Q2DViewerExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    Q2DViewerExtension *q2dviewerExtension;

    if (!(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    // Si no tenim cap volum, alertem a l'usuari
    if (!input)
    {
        QMessageBox::information(0, ApplicationNameString, tr("There is not any viewable or supported Series in the selected Studies"));
    }

    q2dviewerExtension->setPatient(extensionContext.getPatient());
#ifdef STARVIEWER_LITE
    // Assignem l'input per defecte al visor
    q2dviewerExtension->setInput(input);
#endif

    return true;
}

bool Q2DViewerExtensionMediator::reinitializeExtension(QWidget *extension)
{
    Q2DViewerExtension *q2dviewerExtension;

    if (!(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)))
    {
        return false;
    }
    else
    {
#ifndef STARVIEWER_LITE
        q2dviewerExtension->onPatientUpdated();
#endif
        return true;
    }
}

} // End udg namespace
