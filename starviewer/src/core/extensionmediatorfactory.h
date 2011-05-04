#ifndef UDGEXTENSIONMEDIATORFACTORY_H
#define UDGEXTENSIONMEDIATORFACTORY_H

#include <QString>

#include "genericfactory.h"
#include "singleton.h"
#include "extensionmediator.h"

namespace udg {

typedef Singleton<GenericFactory<ExtensionMediator, QString> > ExtensionMediatorFactory;

}

#endif
