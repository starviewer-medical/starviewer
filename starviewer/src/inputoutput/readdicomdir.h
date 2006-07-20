/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREADDICOMDIR_H
#define UDGREADDICOMDIR_H

#define HAVE_CONFIG_H 1
#include "dcdicdir.h" //llegeix el dicom dir

class string;
class DcmDicomDir;

namespace udg {

class Status;
class StudyList;
class SeriesList;
class StudyMask;
class Study;

/** Aquesta classe permet llegir un dicomdir i consultar-ne els seus elements.
  * Accedint a través de l'estructura d'arbres que representen els dicomdir Pacient/Estudi/Series/Imatges, accedim a la informació el Dicomdir per a realitzar cerques.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ReadDicomdir{
public:

    ReadDicomdir();

    /** Obre un directori domcidr
     * @param dicomdirPath directori on es troba el dicomdir
     * @return estat del mètode
     */
    Status open( std::string dicomdirPath );
    
    /** Retorna la llista d'estudis que conté el dicomdir
      * @param studyList llista amb els estudis que conté el dicomdir
      * @param studyMask màscara de cerca dels estudis a cercar dins el dicomdir
      * @return estat del mètode
      */
    Status readStudies( StudyList &studyList , StudyMask studyMask );
    
    /** Retorna la llista de sèries d'un estudi que estigui en el dicomdir
     * @param studyUID UID de l'estudi del qual es vol consultar les sèries
     * @param seriesList llista amb les sèries que conté l'estudi
     * @return estat del mètode
     */
    Status readSeries ( std::string studyUID , SeriesList  &serieList );

    ~ReadDicomdir();

private :

    DcmDicomDir *dicomdir;

    /** Comprova si un estudi compleix la màscara, pels camps PatientId, StudyID, StudyDate, PatientName i AccessionNumber
     * @param study dades de l'estudi
     * @param studyMask màscara de l'estudi
     * @return cert si l'estudi complei la màscara
     */
    bool matchStudyMask( Study study , StudyMask studyMask );

    /** Comprova que els dos StudyId el de la màscara i el de l'estudi siguin iguals. Si l'estudi Id de la màscara està buit, per defecte retorna cert
     * @param studyMaskStudyId studyId de la màscara
     * @param studyStudyId studyId de l'estudi trobat al dicomdir
     * @return retorna cert si els dos studyID son iguals o studyMaskStudyID està buit
     */
    bool matchStudyMaskStudyId( std::string studyMaskStudyId , std:: string studyStudyId );

    /** Comprova que els dos PatientId el de la màscara i el de l'estudi siguin iguals. Si el Patient Id de la màscara està buit, per defecte retorna cert
     * @param studyMaskPatientId 
     * @param studyPatientId 
     * @return retorna cert si els dos patientId són iguals o studyMaskPatientId està buit
     */
    bool matchStudyMaskPatientId( std::string studyMaskPatientId , std:: string studyPatientId );


    /** Comprova que la data de la màscara i la de l'estudi facin matching. Si la studyMaskDate és buida retorna cert per defecte
     * @param studyMaskDate Màscara de dates 
     * @param studyDate Data de l'estudi
     * @return retorna cert si es fa matching amb la data de la màscara o studyMaskDate és buit
     */
    bool matchStudyMaskDate( std::string studyMaskDate , std::string studyDate );

    /** Comprova que el nom del pacient de la màscara i el de l'estudi facin matching. Si la studyMaskPatientName és buida retorna cert per defecte. En aquest cas fem wildcard matching 
     * @param studyMaskPatienName Màscara de nom pacient
     * @param studyPatientName Nom del pacient
     * @return retorna cert si es fa matching amb el nom del pacient de la màscara o studyMaskPatient és buit
     */
    bool matchStudyMaskPatientName( std::string studyMaskPatientName , std::string studyPatientName );

    /** Converteix un string a majúscules,
     *  Com el string distingeix entre majúscules i minúscules, per fer els match, primer convertirem l'string a majúscules, ja que el DICOM guardar la informació en majúscules
     * @param original cadena original
     * @return cadena en majúscules
     */
    std::string upperString( std:: string original );
};

}

#endif
