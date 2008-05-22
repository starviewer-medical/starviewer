/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMDIRREADER_H
#define UDGDICOMDIRREADER_H

#include <QString>

class DcmDicomDir;

namespace udg {

class Status;
class StudyList;
class SeriesList;
class ImageList;
class DicomMask;
class DICOMStudy;

/** Aquesta classe permet llegir un dicomdir i consultar-ne els seus elements.
  * Accedint a través de l'estructura d'arbres que representen els dicomdir Pacient/Estudi/Series/Imatges, accedim a la informació el Dicomdir per a realitzar cerques.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMDIRReader{
public:

    DICOMDIRReader();
    ~DICOMDIRReader();

    /** Obre un directori domcidr
     * @param dicomdirPath directori on es troba el dicomdir
     * @return estat del mètode
     */
    Status open(const QString &dicomdirFilePath);

    /** Retorna la llista d'estudis que conté el dicomdir
      * @param studyList llista amb els estudis que conté el dicomdir
      * @param studyMask màscara de cerca dels estudis a cercar dins el dicomdir
      * @return estat del mètode
      */
    Status readStudies( StudyList &studyList , DicomMask studyMask );

    /** Retorna la llista de sèries d'un estudi que estigui en el dicomdir
     * @param studyUID UID de l'estudi del qual es vol consultar les sèries
     * @param seriesList llista amb les sèries que conté l'estudi
     * @return estat del mètode
     */
    Status readSeries ( QString studyUID , QString seriesUID , SeriesList  &serieList );

    /** Retorna la llista d'imatges que conté un estudi
     * @param seriesUID UID de la serie que volem obtenir les imatges
     * @param imageList Llistat de les imatges que conté
     * @return estat del mètode
     */
    Status readImages( QString seriesUID , QString sopInstanceUID , ImageList &imageList );

    /** Retorna el path del dicomdir
     * @return path del dicomdir
     */
    QString getDicomdirFilePath();

    /**
     * Ens retorna tots els arxius que formen aquell estudi
     * TODO mètode de conveniència per no haver de fer bucles raros a queryscreen i treure una dependència més de DICOMSeries/Study, etc
     * @param studyUID UID de l'estudi del qual volem els arxius
     * @return Una llista amb els paths absoluts dels arxius en qüestió
     */
    QStringList getFiles( QString studyUID );

private :

    DcmDicomDir *m_dicomdir;
    QString m_dicomdirAbsolutePath, m_dicomdirFileName;
    bool m_dicomFilesInLowerCase;

    /** Comprova si un estudi compleix la màscara, pels camps PatientId, StudyID, StudyDate, PatientName i AccessionNumber
     * @param study dades de l'estudi
     * @param studyMask màscara de l'estudi
     * @return cert si l'estudi complei la màscara
     */
    bool matchStudyMask( DICOMStudy study , DicomMask studyMask );

    /** Comprova que els dos StudyId el de la màscara i el de l'estudi siguin iguals. Si l'estudi Id de la màscara està buit, per defecte retorna cert
     * @param studyMaskStudyId studyId de la màscara
     * @param studyStudyId studyId de l'estudi trobat al dicomdir
     * @return retorna cert si els dos studyID son iguals o studyMaskStudyID està buit
     */
    bool matchStudyMaskStudyId( QString studyMaskStudyId , QString studyStudyId );

    /** Comprova que els dos StudyUID el de la màscara i el de l'estudi facin matching. Si l'estudi UID de la màscara està buit, per defecte retorna cert. En aquest cas fem wildcard matching
     * @param studyMaskStudyUID studyUID de la màscara
     * @param studyStudyUID studyUID de l'estudi trobat al dicomdir
     * @return retorna cert si els dos studyUID son iguals o studyMaskStudyUID està buit
     */
    bool matchStudyMaskStudyUID( QString studyMaskStudyUID , QString studyStudyUID );

    /** Comprova que els dos PatientId el de la màscara i el de l'estudi facin matching. Si el Patient Id de la màscara està buit, per defecte retorna cert. En aquest cas fem wildcard matching
     * @param studyMaskPatientId
     * @param studyPatientId
     * @return retorna cert si els dos patientId són iguals o studyMaskPatientId està buit
     */
    bool matchStudyMaskPatientId( QString studyMaskPatientId , QString studyPatientId );

    /** Comprova que la data de la màscara i la de l'estudi facin matching. Si la studyMaskDate és buida retorna cert per defecte
     * @param studyMaskDate Màscara de dates
     * @param studyDate Data de l'estudi
     * @return retorna cert si es fa matching amb la data de la màscara o studyMaskDate és buit
     */
    bool matchStudyMaskDate( QString studyMaskDate , QString studyDate );

    /** Comprova que el nom del pacient de la màscara i el de l'estudi facin matching. Si la studyMaskPatientName és buida retorna cert per defecte. En aquest cas fem wildcard matching
     * @param studyMaskPatienName Màscara de nom pacient
     * @param studyPatientName Nom del pacient
     * @return retorna cert si es fa matching amb el nom del pacient de la màscara o studyMaskPatient és buit
     */
    bool matchStudyMaskPatientName( QString studyMaskPatientName , QString studyPatientName );

    /** Comprova que el AccessionNumber de la màscara i el de l'estudi facin matching. Si la studyMaskAccessionNumber és buida retorna cert per defecte. En aquest cas fem wildcard matching
     * @param studyMaskPatienName Màscara de AccessionNumber
     * @param studyPatientName AccessionNumber de l'estudi
     * @return retorna cert si es fa matching amb studyAccessionNumber de la màscara o studyMaskAccessionNumber és buit
     */
    bool matchStudyMaskAccessionNumber( QString studyMaskAccessionNumber , QString studyAccessionNumber );

    /** canvia les '\' per '/'. Això es degut a que les dcmtk retornen el path de la imatge en format Windows amb els directoris separats per '\'. En el cas de linux les hem de passar a '/'
     * TODO aquest mètode es pot substituir per QDir::toNativeSeparators() o similar que retornarà els separadors adequats al sistema
     * @param original path original
     * @return path amb '/'
     */
    QString backSlashToSlash( QString original );
};

}

#endif
