#ifndef UDGEXTENSIONFACTORY_H
#define UDGEXTENSIONFACTORY_H

#include "genericfactory.h"
#include "singleton.h"

#include <QString>
#include <QWidget>

namespace udg {

/** \fn typedef Singleton<GenericFactory<QWidget, QString> > ExtensionFactory
    Typedef per la classe que serveix per crear una extensió en temps d'execució. Aquesta és d'ús intern a l'hora de registrar una extensió.
*/

typedef Singleton<GenericFactory<QWidget, QString, QWidget> > ExtensionFactory;

}

#endif
