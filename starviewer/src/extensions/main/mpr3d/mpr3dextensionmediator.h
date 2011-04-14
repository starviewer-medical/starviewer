#ifndef MPR3DEXTENSIONMEDIATOR_H
#define MPR3DEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmpr3dextension.h"

namespace udg{

class MPR3DExtensionMediator: public ExtensionMediator
{
    Q_OBJECT
public:
    MPR3DExtensionMediator(QObject *parent = 0);

    ~MPR3DExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPR3DExtension, MPR3DExtensionMediator> registerMPR3DExtension;


} //udg namespace

#endif
