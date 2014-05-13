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

#ifndef UDGEXTENSIONFACTORY_H
#define UDGEXTENSIONFACTORY_H

#include "genericfactory.h"
#include "singleton.h"

#include <QString>
#include <QWidget>

namespace udg {

/**
    \fn typedef Singleton<GenericFactory<QWidget, QString> > ExtensionFactory
    Typedef per la classe que serveix per crear una extensió en temps d'execució. Aquesta és d'ús intern a l'hora de registrar una extensió.
  */
typedef Singleton<GenericFactory<QWidget, QString, QWidget> > ExtensionFactory;

}

#endif
