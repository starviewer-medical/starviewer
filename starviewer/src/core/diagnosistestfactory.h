#ifndef UDGDIAGNOSISTESTFACTORY_H
#define UDGDIAGNOSISTESTFACTORY_H

#include "genericfactory.h"
#include "singleton.h"
#include "diagnosistest.h"

#include <QString>

namespace udg {

/**
    \fn typedef Singleton<GenericFactory<DiagnosisTest, QString> > DiagnosisTestFactory
    Typedef per la classe que serveix per crear un test de diagnosis en temps d'execució. Aquesta és d'ús intern a l'hora de registrar un test de diagnosis.
  */
typedef Singleton<GenericFactory<DiagnosisTest, QString> > DiagnosisTestFactory;

}

#endif
