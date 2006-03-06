/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGPACSNETWORK_H
#define UDGPACSNETWORK_H

#define HAVE_CONFIG_H 1
#include <assoc.h>
#include "status.h"

/** Gestiona els diferents networks que necessitarem per conectar als diferents PACS, el network, só objectes que configuren la nostra xarxa, indiquem
 * per quin port esperem rebre la informació, si serà d'entrada/sortida, etc.. Pel nostre cas tindrem dos tipus de network, un per a fer querys al pacs
 * i un altre pels retrieves d'imatges
 *
 * Aquesta classe es un siglenton, ja que totes les connexions sempre ha d'utitlitzar el mateix network, no podem tenir diferents networks per al mateix 
 * propòsit
 */

namespace udg {

/**
@author marc
*/
class PacsNetwork{
public:

    static PacsNetwork* getPacsNetwork();   
 
    Status createNetworkQuery(int time);
    Status createNetworkRetrieve(int port,int timeout);
   
    T_ASC_Network * getNetworkQuery();
    T_ASC_Network * getNetworkRetrieve();
    
private :

    static PacsNetwork *pacsNetwork;    
    T_ASC_Network *m_networkQuery, *m_networkRetrieve;
  
    
    void disconnect();
    
    PacsNetwork();
    ~PacsNetwork();


};

};
#endif
