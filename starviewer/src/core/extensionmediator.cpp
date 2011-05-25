#include "extensionmediator.h"
#include "logging.h"

namespace udg {

ExtensionMediator::ExtensionMediator(QObject *parent)
 : QObject(parent)
{
}

ExtensionMediator::~ExtensionMediator()
{
}

bool ExtensionMediator::reinitializeExtension(QWidget *extension)
{
    Q_UNUSED(extension);
    return true;
}

};
