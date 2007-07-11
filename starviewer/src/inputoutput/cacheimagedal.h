/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEIMAGEDAL_H
#define UDGCACHEIMAGEDAL_H

#include "imagelist.h"

namespace udg {

class DicomMask;
class Status;
class DICOMImage;

/** Aquesta classe conté els mètodes per operar amb l'objecte image en la caché de l'aplicació
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class CacheImageDAL
{
public:

    CacheImageDAL();

    /** Insereix la informació d'una imatge a la caché, actualitzamt l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat. Per això tot i que accedim a dos taules, al haver-se de fer dins el marc d'una transacció, necessitem fer-les
    dins el mateix mètode. Ja que sinó ens podríem trobar que altres operacions entressin entre insertar la imatge i updatar la pool i quedessin incloses dins la tx
     * @param dades de la imatge
     * @return retorna estat del mètode
     */
    Status insertImage( DICOMImage * );

    /** Cerca les imatges demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID i el SeriesUID de la màscara per fer la cerca, els altres caps de la màscara els ignorarà!
     * @param  mascara de les imatges a cercar
     * @param llistat amb les imatges trobades
     * @return retorna estat del mètode
     */
    Status queryImages( DicomMask mask , ImageList &list );

    /** compta les imatges d'una sèrie
     * @param dicomMask mascarà de les imatges a comptar les images. Las màscara ha de contenir el UID de l'estudi i opcionalment el UID de la sèrie
     * @param imageNumber conte el nombre d'imatges
     * @return retorna estat del mètode
     */
    Status countImageNumber( DicomMask mask , int &imageNumber );

    /** compta lo que ocupen les imatges que compleixen la màscara de cerca
     * @param dicomMask mascarà de les imatges a comptar les images. Las màscara ha de contenir el UID de l'estudi i opcionalment el UID de la sèrie
     * @param size Mida de les imatges que compleixen la màscara en bytes
     * @return retorna estat del mètode
     */
    Status imageSize( DicomMask mask , unsigned long &size );


    /** Esborra les imatges que tinguin el studyUID passat per paràmetre
      * @param studyUID StudyUID de les imatges a esborrar
      */
    Status deleteImages( QString studyUID );

    ///comprova si existeix una imatge
    Status existImage( DicomMask mask, bool &exist );

    ~CacheImageDAL();

private :

    /** Construeix la sentència per buscar les imatges d'un estudi
     * @param mask màscara de cerca
     * @return sentència sql
     */
    QString buildSqlQueryImages( DicomMask *dicomMask );

    /** Construiex la sentència sql per comptar el nombre d'imatges de la sèrie d'un estudi
     * @param mask màscara per construir la sentència sql
     * @return retorna la sentència Sql
     */
    QString buildSqlCountImageNumber( DicomMask *dicomMask );

    /** Construiex la sentència sql per comptar el tamany ocupat per les imatges que compleixen la màscara
     * @param mask màscara per construir la sentència sql
     * @return retorna la sentència Sql
     */
    QString buildSqlSizeImage( DicomMask *dicomMask );

    //sentencia per si existeix imatge
    QString buildSqlExistImage( DicomMask *imageMask );

};

}

#endif
