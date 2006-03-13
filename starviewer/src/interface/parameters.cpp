/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "parameters.h"

namespace udg{
Parameters::Parameters(QObject *parent, const char *name)
 : QObject( parent )
{
    this->setObjectName( name );
}


Parameters::~Parameters()
{
}

} // end namespace udg
