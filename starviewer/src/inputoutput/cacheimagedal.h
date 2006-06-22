/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEIMAGEDAL_H
#define UDGCACHEIMAGEDAL_H

#include "imagelist.h"

class string;


namespace udg {

class ImageMask;
class Status;
class Image;
/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheImageDAL
{
public:
    CacheImageDAL();

    /** Insereix la informació d'una imatge a la caché. I actualitza l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat
     * @param dades de la imatge 
     * @return retorna estat del mètode
     */
    Status insertImage( Image * );
    
    /** Cerca les imatges demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID i el SeriesUID de la màscara per fer la cerca, els altres caps de la màscara els ignorarà!
     * @param  mascara de les imatges a cercar
     * @param llistat amb les imatges trobades
     * @return retorna estat del mètode
     */
    Status queryImages( ImageMask mask , ImageList &list );

    /** compta les imatges d'una sèrie 
     * @param imageMask mascarà de les imatges a comptar les images. Las màscara ha de contenir el UID de l'estudi i opcionalment el UID de la sèrie 
     * @param imageNumber conte el nombre d'imatges
     * @return retorna estat del mètode  
     */
    Status countImageNumber( ImageMask mask , int &imageNumber );
    
    /** Esborra les imatges que tinguin el studyUID passat per paràmetre
      * @param studyUID StudyUID de les imatges a esborrar
      */
    Status deleteImages( std::string studyUID );
    
    ~CacheImageDAL();

private :


    /** Construeix la sentència per buscar les imatges d'un estudi
     * @param mask màscara de cerca
     * @return sentència sql
     */    
    std::string buildSqlQueryImages( ImageMask *imageMask );
    
    /** Construiex la sentència sql per comptar el nombre d'imatges de la sèrie d'un estudi
     * @param mask 
     * @return retorna la sentència Sql
     */
    std::string buildSqlCountImageNumber( ImageMask *imageMask );    


};

}

#endif
