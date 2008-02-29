/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "representation.h"
#include <QString>

namespace udg {

Representation::Representation( QObject *parent ) 
    : QObject( parent ) 
{
    //assignem els valors per defecte que ens interessen
    visibilityOn();
}

QString Representation::getRepresentationType()
{
    return("Representation");
}

};  // end namespace udg
