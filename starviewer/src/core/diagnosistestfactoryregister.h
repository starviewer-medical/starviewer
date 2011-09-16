#ifndef UDGDIAGNOSISTESTFACTORYREGISTER_H
#define UDGDIAGNOSISTESTFACTORYREGISTER_H

#include <QString>

#include "genericsingletonfactoryregister.h"
#include "diagnosistestfactory.h"
#include "logging.h"
#include <iostream>

namespace udg {

/**
    Classe que ens permet registrar un DiagnosisTest en el DiagnosisTestFactory. Per tal de poder registrar un test de diagnosis hem de declarar
    un objecte del tipus DiagnosisTestFactoryRegister.
    Exemple:
    @code
    DiagnosisTestFactoryRegister<DiagnosisTestName> registerAs("DiagnosisTest Identifier");
    @endcode
    Amb aquesta simple línia de codi ja tenim registrat el test de diagnostic en el factory.
  */
template <class FactoryType>
class DiagnosisTestFactoryRegister : public GenericSingletonFactoryRegister<DiagnosisTest, FactoryType, QString, DiagnosisTestFactory, QObject> {
public:
    /// Mètode
    DiagnosisTestFactoryRegister(const QString &identifier)
     : GenericSingletonFactoryRegister<DiagnosisTest, FactoryType, QString, DiagnosisTestFactory, QObject>(identifier)
    {
        DEBUG_LOG("ExtensionFactoryRegister" + identifier);
    }
};

}

#endif
