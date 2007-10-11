/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensionmediator.h"

#include "logging.h"

namespace udg{

ExtensionMediator::ExtensionMediator(QObject *parent)
 : QObject(parent)
{
    DEBUG_LOG( "ExtensionMediator::ExtensionMediator(parent)" );
}


ExtensionMediator::~ExtensionMediator()
{
}

};
