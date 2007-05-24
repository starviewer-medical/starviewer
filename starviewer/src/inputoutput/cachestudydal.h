/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHESTUDYDAL_H
#define UDGCACHESTUDYDAL_H

class string;

namespace udg {

class Study;
class Status;
class StudyList;
class DicomMask;

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheStudyDAL
{
public:

    CacheStudyDAL();

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
    Status insertStudy( Study * );

     /*  Insereix estudi
     *
     * @param Study[in]  Informació de l'estudi
     * @return retorna l'estat de l'inserció
     */
    Status insertStudyDicomdir( Study * );


    /** Cerca els estudis que compleixen la màscara a la caché
     * @param Màscara de la cerca
     * @param StudyList amb els resultats
     * @return retorna estat del mètode
     */
    Status queryStudy( DicomMask mask , StudyList &list );

    /** Cerca l'estudi que compleix amb la màscara de cerca. Cerca ens els estudis que estan en estat Retrived o Retrieving
     * @param  Màscara de  la cerca
     * @param  StudyList amb els resultats
     * @return retorna estat del mètode
     */
    Status queryStudy( std::string studyUID , Study &study );

    /** Selecciona els estudis vells que no han estat visualitzats des de una data inferior a la que es passa per parametre
     * @param Data a partir de la qual es seleccionaran els estudis vells
     * @param StudyList amb els resultats dels estudis, que l'ultima vegada visualitzats es una data inferior a la passa per paràmetre
     * @return retorna estat del mètode
     */
    Status queryOldStudies( std::string OldStudiesDate, StudyList &list );

    /** Esborra un estudi de la cache, l'esborra la taula estudi,series, i image, i si el pacient d'aquell estudi, no té cap altre estudi a la cache local tambe esborrem el pacient. Aquesta operació és dur a terme en una transacció, si falla el mètode, es tirara endarrera, fins a l'estat estable de la base de dades. En aquest mètode es fa accessos a altres taules de la caché com imatge, sèrie, etc.. això és degut a que les operacions d'esborrar un estudi com que afecta a altres taules, s'han de fer una transacció, per això s'ha de tenir juntes en un mateix mètode. Ja que si durant la invocació als diferents mètodes, n'entressin altres mètodes ajens a la operació esborrar estudi, quedarien dins la transacció quan no hi pertanyen. Degut aquest motiu tots es realitzen dins el mateix mètode
     * @param std::string UID de l'estudi
     * @return estat de l'operació
     */
    Status delStudy( std::string );

    /** Aquesta acció es per mantenir la integritat de la base de dades, si ens trobem estudis al iniciar l'aplicació que tenen l'estat pendent o descarregant vol dir que l'aplicació en l'anterior execussió ha finalitzat anòmalament, per tant aquest estudis en estat pendents, les seves sèrie i imatges han de ser borrades perquè es puguin tornar a descarregar.      * @return estat de l'operació
     */
    Status delNotRetrievedStudies();

    /** actualitza l'última vegada que un estudi ha estat visualitzat, d'aquesta manera quant haguem d'esborrar estudis
     * automàticament per falta d'espai, esborarrem els que fa més temps que no s'han visualitzat.
     * @param UID de l'estudi
     * @param hora de visualització de l'estudi format 'hhmm'
     * @param data visualització de l'estudi format 'yyyymmdd'
     * @return estat el mètode
     */
    Status updateStudyAccTime( std::string studyUID );

    /** Updata la modalitat d'un estudi
     * @param Uid de l'estudi a actualitzar
     * @param Modalitat de l'estudi
     * @return retorna estat del mètode
     */
    Status setStudyModality( std::string studyUID , std::string modality );

    /** Updata un estudi PENDING a RETRIEVING, per indicar que l'estudi s'esta descarregant
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieving( std::string studyUID );

    /** Updata un estudi a l'estat RETRIEVED, per indicar que s'ha descarregat
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieved( std::string studyUID );

    /** Updata les dades de l'estudi i pacient que se li passa per paràmetre. El campl clau per buscar l'estudi a actualitzar és l'UID
     *
     * @param updateStudy
     * @return
     */
    Status updateStudy( Study updateStudy );

    ~CacheStudyDAL();

private :

    /** Construeix la sentència sql per fer la query de l'estudi en funció dels parametres de cerca
     * @param mascara de cerca
     * @return retorna estat del mètode
     */
    std::string buildSqlQueryStudy( DicomMask *studyMask );

    /** Converteix l'asterisc, que conte el tag origen per %, per transformar-lo a la sintaxis de sql
     * @param string original
     * @return retorna l'string original, havent canviat els '*' per '%'
     */
    std::string replaceAsterisk( std::string original );

    /** retorna l'hora del sistema
     * @return retorna l'hora del sistema en format HHMM
     */
    int getTime();

    /** retorna la data del sistema
     *    @return retorna la data del sistema en format yyyymmdd
     */
    int getDate();

    /** Passant-li la màscara d'estudi construeix la sentència sql per a buscar per modalitat, si s'ha especificat modalitat
     * Les modalitats han d'estar separades per ','
     * @param màscara
     * @return sentència sql
     */
    std::string buildSqlStudyModality( DicomMask * mask );

};
}

#endif
