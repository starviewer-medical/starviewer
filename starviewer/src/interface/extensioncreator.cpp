/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensioncreator.h"

namespace udg {

ExtensionCreator::ExtensionCreator( QObject *parent, const char *name ) 
: QObject( parent )
{
    this->setObjectName( name );
}


ExtensionCreator::~ExtensionCreator()
{
}


};  // end namespace udg {
