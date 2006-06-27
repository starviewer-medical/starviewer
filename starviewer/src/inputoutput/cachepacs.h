/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGCACHEPACS_H
#define UDGCACHEPACS_H

#include <sqlite.h>
#include "studylist.h"
#include "serieslist.h"
#include "imagelist.h"
#include "databaseconnection.h"

class string;

namespace udg {

/** Classe que gestiona la caché de l'aplicació, i fa els inserts,queries i deletre
@author marc
*/

class Study;
class Series;
class Image;
class StudyMask;
class SeriesMask;
class ImageMask;
class Status;

class CachePacs
{

private:
    
    DatabaseConnection *m_DBConnect;

    ///Constructor de la classe        
    CachePacs();
    
    ///Destructor de la classe
    ~CachePacs();

        
   /** Construeix l'estat en que ha finaltizat l'operació sol·licitada
    * @param  Estat de sqlite
    * @return retorna l'estat de l'operació
    */
    Status constructState(int error);

public:
 
     static CachePacs* getCachePacs()
     {
         static CachePacs cache;
         return &cache; 
     }
    
    
};

};

#endif
