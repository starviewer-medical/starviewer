
#ifndef UDGIMAGEPRINTEXTENSIONMEDIATOR_H
#define UDGIMAGEPRINTEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qimageprintextension.h"

namespace udg {

class ImagePrintExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    ImagePrintExtensionMediator(QObject *parent = 0);

    ~ImagePrintExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QImagePrintExtension, ImagePrintExtensionMediator> registerImagePrintExtension;
}

#endif
