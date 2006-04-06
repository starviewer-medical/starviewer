/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHELAYER_H
#define UDGCACHELAYER_H

#include <QObject>
#include "status.h"

namespace udg {

/**     Aquesta classe afegeix un nivell d'abstracio sobre la classe CachePacs, permetent realitzar operacions sobre la cache, i utilitzar elements de la interfcie
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheLayer : public QObject
{


Q_OBJECT
public:
    /** Constructor de la classe
      */
    CacheLayer(QObject *parent = 0);
    
    /** Neteja la cache de l'aplicacio, Mostra un QProgressDialog amb el progress de la neteja
      */
    Status clearCache();
    
    /**Destructor de la classe
      */
    ~CacheLayer();

};

}

#endif
