#ifndef MPREXTENSIONMEDIATOR_H
#define MPREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmprextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPRExtensionMediator: public ExtensionMediator
{
    Q_OBJECT
public:
    MPRExtensionMediator(QObject *parent = 0);

    ~MPRExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPRExtension, MPRExtensionMediator> registerMPRExtension;


} //udg namespace

#endif
