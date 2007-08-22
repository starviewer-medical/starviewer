/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDREPRESENTATION_H
#define UDGDREPRESENTATION_H

#include <QObject>

//Forward declarations

namespace udg {
//Forward declarations
    
/**
Classe base per a totes les sub-classes que encapsulen dades necessàries per a les diferents representacions

@author Grup de Gràfics de Girona  ( GGG )
*/

class ToolData : public QObject{
    Q_OBJECT
public:
    
    ToolData( QObject *parent = 0 );
    ~ToolData();
};

};  
#endif

