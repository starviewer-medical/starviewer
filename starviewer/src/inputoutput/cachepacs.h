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

    /** Construeix la sentència sql per fer la query de l'estudi en funció dels parametres de cerca
     * @param mascara de cerca
     * @return retorna estat del mètode
     */
    std::string buildSqlQueryStudy( StudyMask *studyMask );

    /** Construeix la sentència per buscar les sèries d'un estudi
     * @param mask màscara de cerca
     * @return sentència sql
     */    
	std::string buildSqlQuerySeries( SeriesMask *seriesMask );
    
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
	            
    /** retorna l'hora del sistema
     * @return retorna l'hora del sistema en format HHMM
     */
    int getTime();
    
	/** retorna la data del sistema
     *    @return retorna la data del sistema en format yyyymmdd
     */
    int getDate();
    
    /** Converteix l'asterisc, que conte el tag origen per %, per transformar-lo a la sintaxis de sql
     * @param string original
     * @return retorna l'string original, havent canviat els '*' per '%'
     */
    std::string replaceAsterisk( std::string );

public:
 
     static CachePacs* getCachePacs()
     {
         static CachePacs cache;
         return &cache; 
     }
   
    /** Afegeix un nou estudi i pacient a la bd local, quant comencem a descarregar un nou estudi.
     *   La informació que insereix és :
     *        Si el pacient no existeix - PatientId
     *                                  - PatientName
     *                                  - PatientBirthDate
     *                                  - PatientSex  
     *
     *       Si l'estudi no existeix    - PatientID
     *                                  - StudyUID
     *                                  - StudyDate
     *                                  - StudyTime
     *                                  - StudyID
     *                                  - AccessionNumber
     *                                  - StudyDescription
     *                                  - Status
     *  La resta d'informació no estarà disponible fins que les imatges estiguin descarregades, 
     *                    
     * @param Study[in]  Informació de l'estudi 
     * @return retorna l'estat de l'inserció                                    
     */
    Status insertStudy(Study *); 

    /** Insereix una sèrie a la caché
     * @param series Dades de la sèrie
     * @return retorna l'estat de la inserció
     */
    Status insertSeries(Series *); 

    /** Insereix la informació d'una imatge a la caché. I actualitza l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat
     * @param dades de la imatge 
     * @return retorna estat del mètode
     */
    Status insertImage(Image *);

    /** Cerca els estudis que compleixen la màscara a la caché
     * @param Màscara de la cerca
     * @param StudyList amb els resultats
     * @return retorna estat del mètode
     */
    Status queryStudy(StudyMask mask,StudyList &list);

    /** Cerca l'estudi que compleix amb la màscara de cerca. Cerca ens els estudis que estan en estat Retrived o Retrieving
     * @param    Màscara de  la cerca
     * @param    StudyList amb els resultats
     * @return retorna estat del mètode
     */
    Status queryStudy(std::string studyUID,Study &study);

    /** Selecciona els estudis vells que no han estat visualitzats des de una data inferior a la que es passa per parametre
     * @param  Data a partir de la qual es seleccionaran els estudis vells
     * @param  StudyList amb els resultats dels estudis, que l'ultima vegada visualitzats es una data inferior a la passa per paràmetre
     * @return retorna estat del mètode
     */
    Status queryOldStudies( std::string , StudyList &list );

    /** Cerca les sèries demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID de la màscara per fer la cerca, els altres camps de la màscara els ignorarà!
     * @param  mascarà de la serie amb l' sstudiUID a buscar
     * @param  retorna la llista amb la sèries trobades
     * @return retorna estat del mètode
     */
    Status querySeries(SeriesMask mask,SeriesList &list);

    /** Cerca les imatges demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID i el SeriesUID de la màscara per fer la cerca, els altres caps de la màscara els ignorarà!
     * @param  mascara de les imatges a cercar
     * @param llistat amb les imatges trobades
     * @return retorna estat del mètode
     */
    Status queryImages(ImageMask mask,ImageList &list);

    /** compta les imatges d'una sèrie 
     * @param series [in] mascarà de la serie a comptar les images. Las màscara ha de contenir el UID de l'estudi i de la sèrie
     * @param imageNumber [out] conte el nombre d'imatges
     * @return retorna estat del mètode  
     */
    Status countImageNumber(ImageMask mask,int &imageNumber);
    
    /** actualitza l'última vegada que un estudi ha estat visualitzat, d'aquesta manera quant haguem d'esborrar estudis
     * automàticament per falta d'espai, esborarrem els que fa més temps que no s'han visualitzat
     * @param UID de l'estudi
     * @param hora de visualització de l'estudi format 'hhmm'
     * @param data visualització de l'estudi format 'yyyymmdd'
     * @return estat el mètode
     */
    Status updateStudyAccTime(std::string studyUID);
    
    /** Updata un estudi a Retrieved
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieved(std::string studyUID);
    
    /** Updata la modalitat d'un estudi
     * @param Uid de l'estudi a actualitzar
     * @param Modalitat de l'estudi
     * @return retorna estat del mètode
     */
    Status setStudyModality(std::string studyUID,std::string modality);
    
    /** Updata un estudi PENDING a RETRIEVING, per indicar que l'estudi s'esta descarregant
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieving(std::string studyUID);
    
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
