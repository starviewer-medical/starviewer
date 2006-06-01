/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include <string> 

#include "processimagesingleton.h"
#include "processimage.h"
#include "image.h"
namespace udg {

/** Constructor de la classe
  */
ProcessImageSingleton::ProcessImageSingleton()
{
    int init_value = 1;//Només un thread alhora pot gravar a la llista
    m_semafor = (sem_t*)malloc(sizeof(sem_t));
    sem_init(m_semafor,0,init_value);    
}

ProcessImageSingleton* ProcessImageSingleton::pInstance = 0;

/** retorna una instancia de l'objecte
  *        @return instancia de l'objecte
  */
ProcessImageSingleton* ProcessImageSingleton::getProcessImageSingleton()
{
    if (pInstance == 0)
    {
        pInstance = new ProcessImageSingleton;
    }
    
    return pInstance;
}

/** Afegeix un nou objecte ProcessImage, que s'encarregarrà de processar la informació de cada imatge descarregada
  *        @param    UID de l'estudi
  *        @param    Objecte processimage que tractarà la descarrega d'imatges
  */
void ProcessImageSingleton::addNewProcessImage(std::string UID,ProcessImage *pi)
{
    SingletonProcess sp;
    
    sp.studyUID = UID;
    sp.imgProcess = pi;
    
    sem_wait(m_semafor);   
    m_listProcess.push_back(sp);
    sem_post(m_semafor);
        
}

/** buscar l'objecte processimage que s'encarrega de gestionar la descarrega de l'estudi studyUID, per processar la imatge
  *        @param     UID de l'estudi que ha produit l'error
  *        @param    imatge descarregada
  */
void ProcessImageSingleton::process(std::string UID,Image* img)
{

    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while (j != m_listProcess.end())
    {
        if ((*j).studyUID != UID)
        {
            j++;
        }
        else break;
    }

    if (j != m_listProcess.end())
    {
        (*j).imgProcess->process(img);
    }
    
}

/** buscar l'objecte processimage que s'encarrega de gestionar la descarrega de l'estudi studyUID, per notificar l'error
  *        @param     UID de l'estudi que ha produit l'error
  */
void ProcessImageSingleton::setErrorRetrieving(std::string studyUID)
{

    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while (j != m_listProcess.end())
    {
        if ((*j).studyUID != studyUID)
        {
            j++;
        }
        else break;
    }

    if (j != m_listProcess.end())
    {    
        (*j).imgProcess->setErrorRetrieving();
    }
    
}

/** esborra el ProcessImage de la llista
  *        @param UID del Process Image de l'estudi a esborrar
  */
bool ProcessImageSingleton::delProcessImage(std::string UID)
{

    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while (j != m_listProcess.end())
    {
        if ((*j).studyUID != UID)
        {
            j++;
        }
        else break;
    }

    if (j != m_listProcess.end())
    {   
        sem_wait(m_semafor);
        m_listProcess.erase(j);
        sem_post(m_semafor);
        return true;
    }
    else return false;
}

/** Estableix el path on s'han de guardar les imatges de la caché
  *        @param path de la cache on es guarden les imatges
  */
void ProcessImageSingleton::setPath(std::string path)
{
    m_imagePath = path;
}

/** retorna el path de la caché on s'han de guardar les imatges
  *        @return path de la cache
  */
std::string ProcessImageSingleton::getPath()
{
    return m_imagePath;
}

/** destructor de la classe
  */
ProcessImageSingleton::~ProcessImageSingleton()
{
}


};
