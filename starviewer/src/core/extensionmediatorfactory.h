/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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
typedef Singleton<GenericFactory<ExtensionMediator, QString> > ExtensionMediatorFactory;

}

#endif
