/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGRETRIEVETHREADSLIST_H
#define UDGRETRIEVETHREADSLIST_H

#include <pthread.h>
#include <list.h>
#include <semaphore.h>

namespace udg {

/** Aquesta classe singleton conte una llista dels estudis que s'estan descarregant en un moment determinat. Aquesta llista es necessaria per controlar els errors
  *estat, que l'usuari no sorti de l'aplicació a mitja descarrega, etc.. Aquesta classe ens permetra tenir un control de tots els threads que estan descarregant
  *en un determinat moment, els podrem esborrar, afegir-ne de nous, coneixer el seu estat, etc..
  * A demés controla el nombre màxim de threads que tenim descarregant imatges alhora, abans de descarregar un estudi han d'agafar torn, i al final
  * de descarregar l'estudi l'alliberen
@author marc
*/

class RetrieveThread;
class RetrieveThreadsList{

private :

    static  RetrieveThreadsList* pInstance;
    
    RetrieveThreadsList();
    ~RetrieveThreadsList();

    
    list<RetrieveThread>threadList;     
    list<RetrieveThread>::iterator i;
    
    sem_t *m_semafor,*m_thread;//semafor per controlar el numero maxim de descarregues simultànies

public:

    static  RetrieveThreadsList* getRetrieveThreadsList();
    
    void getTurn();
    void releaseTurn();
    
    void setMaxThreads(int);
    
    void addThread(RetrieveThread);
    
    bool deleteRetrieveThread(std::string studyUID);    
    
    RetrieveThread getRetrieveThread(std::string studyUID);
    
    bool allThreadsHaveFinished();

};

};

#endif
