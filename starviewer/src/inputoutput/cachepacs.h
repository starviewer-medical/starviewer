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

    /** Insereix la informació d'una imatge a la caché. I actualitza l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat
     * @param dades de la imatge 
     * @return retorna estat del mètode
     */
    Status insertImage(Image *);

    /** Selecciona els estudis vells que no han estat visualitzats des de una data inferior a la que es passa per parametre
     * @param  Data a partir de la qual es seleccionaran els estudis vells
     * @param  StudyList amb els resultats dels estudis, que l'ultima vegada visualitzats es una data inferior a la passa per paràmetre
     * @return retorna estat del mètode
     */
    Status queryOldStudies( std::string , StudyList &list );
    
    /** Esborra un estudi de la cache, l'esborra la taula estudi,series, i image, i si el pacient d'aquell estudi, no té cap altre estudi a la cache local tambe esborrem el pacient
     * @param std::string [in] UID de l'estudi
     * @return estat de l'operació
     */
    Status delStudy(std::string);
    
    /** Aquesta acció es per mantenir la coherencia de la base de dades, si ens trobem estudis al iniciar l'aplicació que tenen l'estat pendent o descarregant vol dir que l'aplicació en l'anterior execussió ha finalitzat anòmalament, per tant aquest estudis en estat pendents, les seves sèrie i imatges han de ser borrades perquè es puguin tornar a descarregar. Aquesta acció és simplement per seguretat!
     * @return estat de l'operació
     */
    Status delNotRetrievedStudies();    
    
    /** Compacta la base de dades de la cache, per estalviar espai
     * @return estat del mètode  
     */
    Status compactCachePacs();
    
};

};

#endif
