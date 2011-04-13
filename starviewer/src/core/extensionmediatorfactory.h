#ifndef UDGEXTENSIONMEDIATORFACTORY_H
#define UDGEXTENSIONMEDIATORFACTORY_H

#include <QString>

#include "genericfactory.h"
#include "singleton.h"
#include "extensionmediator.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
typedef Singleton< GenericFactory<ExtensionMediator, QString> > ExtensionMediatorFactory;

}

#endif
