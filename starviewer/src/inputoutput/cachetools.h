/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHETOOLS_H
#define UDGCACHETOOLS_H

namespace udg {

class Status;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheTools{
public:
    CacheTools();

    /** Compacta la base de dades de la cache, per estalviar espai
     * @return estat del mètode
     */
    Status compactCachePacs();

    ~CacheTools();

};

}

#endif
