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

ProcessImageSingleton::ProcessImageSingleton()
{
    int init_value = 1;//Només un thread alhora pot gravar a la llista
    m_semafor = ( sem_t* ) malloc( sizeof( sem_t ) );
    sem_init( m_semafor , 0 , init_value );    
}

ProcessImageSingleton* ProcessImageSingleton::pInstance = 0;

ProcessImageSingleton* ProcessImageSingleton::getProcessImageSingleton()
{
    if ( pInstance == 0 )
    {
        pInstance = new ProcessImageSingleton;
    }
    
    return pInstance;
}

void ProcessImageSingleton::addNewProcessImage( std::string UID , ProcessImage *pi )
{
    SingletonProcess sp;
    
    sp.studyUID = UID;
    sp.imgProcess = pi;
    
    sem_wait( m_semafor );   
    m_listProcess.push_back( sp );
    sem_post( m_semafor );
}

void ProcessImageSingleton::process( std::string UID , Image* img )
{
    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while ( j != m_listProcess.end() )
    {
        if ( (*j).studyUID != UID )
        {
            j++;
        }
        else break;
    }

    if ( j != m_listProcess.end() )
    {
        (*j).imgProcess->process( img );
    }
}

void ProcessImageSingleton::setError( std::string studyUID )
{
    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while ( j != m_listProcess.end() )
    {
        if ( (*j).studyUID != studyUID )
        {
            j++;
        }
        else break;
    }

    if ( j != m_listProcess.end() )
    {    
        (*j).imgProcess->setError();
    }
}

bool ProcessImageSingleton::delProcessImage( std::string UID )
{
    list<SingletonProcess>::iterator j;
    
    j = m_listProcess.begin();
    
    while ( j != m_listProcess.end() )
    {
        if ( (*j).studyUID != UID )
        {
            j++;
        }
        else break;
    }

    if ( j != m_listProcess.end() )
    {   
        sem_wait( m_semafor );
        m_listProcess.erase( j );
        sem_post( m_semafor );
        return true;
    }
    else return false;
}

void ProcessImageSingleton::setPath( std::string path )
{
    m_imagePath = path;
}

std::string ProcessImageSingleton::getPath()
{
    return m_imagePath;
}

ProcessImageSingleton::~ProcessImageSingleton()
{
}

};
