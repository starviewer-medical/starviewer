/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONMEDIATORFACTORYREGISTER_H
#define UDGEXTENSIONMEDIATORFACTORYREGISTER_H

#include <QString>

#include "extensionmediatorfactory.h"
#include "genericsingletonfactoryregister.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class FactoryType>
class ExtensionMediatorFactoryRegister
                        : public GenericSingletonFactoryRegister<ExtensionMediator, FactoryType, QString, ExtensionMediatorFactory>
{
public:
    ///Mètode
    ExtensionMediatorFactoryRegister(QString identifier)
        :GenericSingletonFactoryRegister<ExtensionMediator, FactoryType, QString, ExtensionMediatorFactory>( identifier )
    {
    }
};

}

#endif
