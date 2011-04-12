
#ifndef UDGDICOMPRINTEXTENSIONMEDIATOR_H
#define UDGDICOMPRINTEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qdicomprintextension.h"

namespace udg {

class DicomPrintExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    DicomPrintExtensionMediator(QObject *parent = 0);

    ~DicomPrintExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QDicomPrintExtension, DicomPrintExtensionMediator> registerImagePrintExtension;
}

#endif
