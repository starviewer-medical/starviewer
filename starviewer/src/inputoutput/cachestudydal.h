/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHESTUDYDAL_H
#define UDGCACHESTUDYDAL_H

namespace udg {

class DICOMStudy;
class Status;
class StudyList;
class DicomMask;

class Study;

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
     * @param study estudi a inserir en la cache
     * @param source indiquem si la font és un PACS o un DICOMDIR. Per defecte serà pacs
     * @return estat de l'operació
     */
    Status insertStudy( DICOMStudy *study, QString source = "PACS" );

    /**
     * Mètode genèric que consulta la base de dades i ens retorna els resultats en una llista on cada element és
     * una "row" i aquesta conté els atributs pels que hem fet el query en una string list
     * TODO aquest hauria de ser un mètode que proporcionés la pròpia entitat de base de dades
     * @param query La consulta en sql que volem fer a la base de dades
     * @param results Llista de resultats. Cada element de la llista equival a un "row" del resultat del query.
     * A cada "row"(element de la llista), tenim les columnes, és a dir una llista d'strings, on cadascun
     * es correspon amb els atributs que hem demanat en la query
     * @return Status de la operació
     */
    Status queryDatabase( QString query , QList<QStringList> &results );
    Status queryDatabase( DicomMask maskQuery , QList<QStringList> &results );

    /** Cerca els estudis que compleixen la màscara a la caché
     * @param Màscara de la cerca
     * @param StudyList amb els resultats
     * @return retorna estat del mètode
     */
    Status queryStudy( DicomMask mask , StudyList &list );

    /** Omple l'estudi amb l'uid donat.
     * @param  studyUID identificador de l'estudi
     * @param  study que té aquest identificador
     * @return retorna estat del mètode
     */
    Status queryStudy( QString studyUID , DICOMStudy &study );

    /**
     * Cerca l'estudi amb uid studyUID i l'omple. Si no troba aquest estudi no omplirà res
     * Mètode transitori per introduir l'estructura Study en detriment de DICOMStudy
     * @param studyUID uid de l'estudi que volem consultar
     * @param study estudi que omplirem amb les dades
     * @return
     */
    Status queryStudy( QString studyUID , Study *study );

    /** Ens retorna les dades del pacient que té l'estudi amb uid studyUID. Només omple la informació de pacient i prou,
     * no omple la informació dels estudis, ni series, etc..
     * @param  studyUID identificador de l'estudi
     * @param  patient que té aquest estudi amb aquest uid
     * @return retorna estat del mètode
     */
//     Status queryPatientWithStudy( QString studyUID , Patient *patient );

    /** Selecciona els estudis vells que no han estat visualitzats des de una data inferior a la que es passa per parametre
     * @param Data a partir de la qual es seleccionaran els estudis vells
     * @param StudyList amb els resultats dels estudis, que l'ultima vegada visualitzats es una data inferior a la passa per paràmetre
     * @return retorna estat del mètode
     */
    Status queryOldStudies( QString OldStudiesDate, StudyList &list );

    /** Selecciona tots els estudis de la base de dades, inclosos els que estan en estat PENDING and RETRIEVING
     * @param StudyList conté tots els estudis de la base de dades, independentment de l'estat
     * @return retorna estat del mètode
     */
    Status queryAllStudies( StudyList &list );

    /** Esborra un estudi de la cache, l'esborra la taula estudi,series, i image, i si el pacient d'aquell estudi, no té cap altre estudi a la cache local tambe esborrem el pacient. Aquesta operació és dur a terme en una transacció, si falla el mètode, es tirara endarrera, fins a l'estat estable de la base de dades. En aquest mètode es fa accessos a altres taules de la caché com imatge, sèrie, etc.. això és degut a que les operacions d'esborrar un estudi com que afecta a altres taules, s'han de fer una transacció, per això s'ha de tenir juntes en un mateix mètode. Ja que si durant la invocació als diferents mètodes, n'entressin altres mètodes ajens a la operació esborrar estudi, quedarien dins la transacció quan no hi pertanyen. Degut aquest motiu tots es realitzen dins el mateix mètode
     * @param QString UID de l'estudi
     * @return estat de l'operació
     */
    Status delStudy( QString );

    /** actualitza l'última vegada que un estudi ha estat visualitzat, d'aquesta manera quant haguem d'esborrar estudis
     * automàticament per falta d'espai, esborarrem els que fa més temps que no s'han visualitzat.
     * @param UID de l'estudi
     * @param hora de visualització de l'estudi format 'hhmm'
     * @param data visualització de l'estudi format 'yyyymmdd'
     * @return estat el mètode
     */
    Status updateStudyAccTime( QString studyUID );

    /** Updata la modalitat d'un estudi
     * @param Uid de l'estudi a actualitzar
     * @param Modalitat de l'estudi
     * @return retorna estat del mètode
     */
    Status setStudyModality( QString studyUID , QString modality );

    /** Updata un estudi PENDING a RETRIEVING, per indicar que l'estudi s'esta descarregant
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieving( QString studyUID );

    /** Updata un estudi a l'estat RETRIEVED, per indicar que s'ha descarregat
     * @param Uid de l'estudi a actualitzar
     * @return retorna estat del mètode
     */
    Status setStudyRetrieved( QString studyUID );

    /** Updata les dades de l'estudi i pacient que se li passa per paràmetre. El campl clau per buscar l'estudi a actualitzar és l'UID
     *
     * @param updateStudy
     * @return
     */
    Status updateStudy( DICOMStudy updateStudy );

    ~CacheStudyDAL();

private :

    /** Construeix la sentència sql per fer la query de l'estudi en funció dels parametres de cerca
     * @param mascara de cerca
     * @return retorna estat del mètode
     */
    QString buildSqlQueryStudy( DicomMask *studyMask );

    /** Converteix l'asterisc, que conte el tag origen per %, per transformar-lo a la sintaxis de sql
     * @param string original
     * @return retorna l'string original, havent canviat els '*' per '%'
     */
    QString replaceAsterisk( QString original );

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
    QString buildSqlStudyModality( DicomMask * mask );

};
}

#endif
