/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSHAPE_H
#define UDGSHAPE_H

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Superclasse de totes les formes possibles que es poden dibuixar sobre un visor. 
        Implementa les operacions fonamentals d'aquestes formes i defineix els atributs
        generals de totes elles.
*/
class Shape{
public:
    Shape();

    ~Shape();
    
    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ("Shape");}

};

}

#endif
