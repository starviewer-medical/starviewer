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

void ExtensionMediator::viewNewStudiesFromSamePatient(QWidget *extension)
{
    Q_UNUSED(extension)
}

};
