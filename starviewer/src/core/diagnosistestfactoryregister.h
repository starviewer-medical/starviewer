/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
        DEBUG_LOG("DiagnosisTestFactoryRegister: " + identifier);
    }
};

}

#endif
