#ifndef UDGEXTENSIONMEDIATORFACTORYREGISTER_H
#define UDGEXTENSIONMEDIATORFACTORYREGISTER_H

#include <QString>

#include "extensionmediatorfactory.h"
#include "genericsingletonfactoryregister.h"

namespace udg {

template <class FactoryType>
class ExtensionMediatorFactoryRegister
                        : public GenericSingletonFactoryRegister<ExtensionMediator, FactoryType, QString, ExtensionMediatorFactory>
{
public:
    ///Mètode
    ExtensionMediatorFactoryRegister(const QString &identifier)
        :GenericSingletonFactoryRegister<ExtensionMediator, FactoryType, QString, ExtensionMediatorFactory>(identifier)
    {
    }
};

}

#endif
