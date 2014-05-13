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
