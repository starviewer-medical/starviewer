/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "retrievethreadslist.h"
#include "retrievethread.h"

namespace udg {

/** Constructor de la classe
  */
RetrieveThreadsList::RetrieveThreadsList()
{
    m_semafor = (sem_t*)malloc(sizeof(sem_t));
    m_thread = (sem_t*)malloc(sizeof(sem_t));

    sem_init(m_semafor,0,1);
}

RetrieveThreadsList* RetrieveThreadsList::pInstance = 0;

/** Retorna una instancia de l'objecte
  *        @return instancia de l'objecte
  */
RetrieveThreadsList* RetrieveThreadsList::getRetrieveThreadsList()
{
    if (pInstance == 0)
    {
        pInstance = new RetrieveThreadsList;
    }
    
    return pInstance;
}

/** Nombre màxim de threads que podem tenir executant-se
  */
void RetrieveThreadsList::setMaxThreads(int threads)
{
    sem_init(m_thread,0,threads);
}

/** s'utilitza perquè el thread demani torn per a poder realtizar les seves accions
  */
void RetrieveThreadsList::getTurn()
{
    sem_wait(m_thread);
}


/** s'utilitza per a que el thread una vegada hagi acabat alliberi el torn per a que d'altres threads el pugin utilitzar
  */
void RetrieveThreadsList::releaseTurn()
{
    sem_post(m_thread);
}

/** Afegeix un Thread a la llista de threads que estan descarregant estudis
  *        @param RetrieveThread[in] thread a guardar
  */
void RetrieveThreadsList::addThread(RetrieveThread rThread)
{
    sem_wait(m_semafor);
    threadList.push_back(rThread);
    sem_post(m_semafor);
}

/** Esborrar el thread encarregat de descarregar l'estudi, de la llista
  *    @param string[in] UID de l'estudi a descarregar
  */
bool RetrieveThreadsList::deleteRetrieveThread(std::string studyUID)
{
    sem_wait(m_semafor);
    i=threadList.begin();
    
    while (i!=threadList.end())
    {
        if ((*i).getStudyUID() ==studyUID)
        {
            break;
        }
        else i++;
    } 
    
    if (i!=threadList.end())
    {
        threadList.erase(i);
        sem_post(m_semafor);
        return true;
    }
    else
    {
        sem_post(m_semafor);
        return false;
    }
}




RetrieveThread RetrieveThreadsList::getRetrieveThread(std::string studyUID)
{
    sem_wait(m_semafor);
    i=threadList.begin();
    RetrieveThread rThread;
    
    while (i!=threadList.end())
    {
        if ((*i).getStudyUID() ==studyUID)
        {
            break;
        }
        else i++;
    } 
    
    if (i!=threadList.end())
    {
        rThread = (*i);
        sem_post(m_semafor);    
    }
    
    return rThread;
}

RetrieveThreadsList::~RetrieveThreadsList()
{

}

};
