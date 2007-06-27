/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONFACTORYREGISTER_H
#define UDGEXTENSIONFACTORYREGISTER_H

#include <QWidget>
#include <QString>

#include "genericsingletonfactoryregister.h"
#include "extensionfactory.h"
#include "logging.h"

namespace udg {

/**
    Classe que ens permet registrar una Extension en el ExtensionFactory. Per tal de poder registrar una extensió hem de declarar
    un objecte del tipus ExtensionFactoryRegister.
    Exemple:
    @code
    ExtensionFactoryRegister<ExtensionName> registerAs("Extension Identifier");
    @endcode
    Amb aquesta simple línia de codi ja tenim registrada la nostra extensió en el factory.

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class FactoryType>
class ExtensionFactoryRegister: public GenericSingletonFactoryRegister<QWidget, FactoryType, QString, ExtensionFactory, QWidget>
{
public:
    ///Mètode
    ExtensionFactoryRegister(QString identifier)
        :GenericSingletonFactoryRegister<QWidget, FactoryType, QString, ExtensionFactory, QWidget>( identifier )
    {
        DEBUG_LOG( "ExtensionFactoryRegister" + identifier );
    }
};

}

#endif
