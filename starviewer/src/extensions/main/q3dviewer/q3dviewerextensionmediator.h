#ifndef UDGQ3DVIEWEREXTENSIONMEDIATOR_H
#define UDGQ3DVIEWEREXTENSIONMEDIATOR_H

#include "extensionmediator.h"
#include "installextension.h"
#include "q3dviewerextension.h"

namespace udg {

/**
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Q3DViewerExtensionMediator : public ExtensionMediator
{
    Q_OBJECT
public:

    Q3DViewerExtensionMediator(QObject * parent = 0);
    ~Q3DViewerExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

};

static InstallExtension<Q3DViewerExtension, Q3DViewerExtensionMediator> registerQ3DViewerExtensionMediator;

}

#endif
