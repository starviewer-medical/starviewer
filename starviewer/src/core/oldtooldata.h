/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOLDTOOLDATA_H
#define UDGOLDTOOLDATA_H

#include <QObject>

//Forward declarations

namespace udg {
//Forward declarations

/**
Classe base per a totes les sub-classes que encapsulen dades necessàries per a les diferents representacions

@author Grup de Gràfics de Girona  ( GGG )
*/

class OldToolData : public QObject{
Q_OBJECT
public:

    OldToolData( QObject *parent = 0 );
    ~OldToolData();
};

};
#endif

