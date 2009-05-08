/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMDIRREADER_H
#define UDGDICOMDIRREADER_H

#include <QString>
#include <QList>

class DcmDicomDir;
class DcmDirectoryRecord;

namespace udg {

class DicomMask;
class Patient;
class Study;
class Series;
class Image;
class Status;

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
      * @param outResultsStudyList llista amb els estudis que conté el dicomdir
      * @param studyMask màscara de cerca dels estudis a cercar dins el dicomdir
      * @return estat del mètode
      */
    Status readStudies( QList<Patient*> &outResultsStudyList , DicomMask studyMask );

    /** Retorna la llista de sèries d'un estudi que estigui en el dicomdir
     * @param studyUID UID de l'estudi del qual es vol consultar les sèries
     * @param outResultsSeriesList llista amb les sèries que conté l'estudi
     * @return estat del mètode
     */
    Status readSeries ( const QString &studyUID , const QString &seriesUID , QList<Series*> &outResultsSeriesList );

    /** Retorna la llista d'imatges que conté un estudi
     * @param seriesUID UID de la serie que volem obtenir les imatges
     * @param imageList Llistat de les imatges que conté
     * @return estat del mètode
     */
    Status readImages( const QString &seriesUID , const QString &sopInstanceUID , QList<Image*> &outResultsImageList );

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
    QStringList getFiles( const QString &studyUID );

    /// Retorna l'estructura Patient per l'estudi que compleixi la màscara que se li passi.
    /// En la màscara només es té en compte el StudyInstanceUID.
    Patient* retrieve(DicomMask maskToRetrieve);

private :

    DcmDicomDir *m_dicomdir;
    QString m_dicomdirAbsolutePath, m_dicomdirFileName;
    bool m_dicomFilesInLowerCase;

    ///Comprova que un pacient compleixi amb la màscara (comprova que compleixi el  Patient Name i Patient ID)
    bool matchPatientToDicomMask( Patient *patient , DicomMask *mask);

    /** Comprova si un estudi compleix la màscara, pels camps StudyUID, StudyDate
     */
    bool matchStudyToDicomMask( Study *study , DicomMask *mask );

    /** Comprova que els dos StudyUID el de la màscara i el de l'estudi facin matching. Si l'estudi UID de la màscara està buit, per defecte retorna cert. En aquest cas fem wildcard matching
     */
    bool matchDicomMaskToStudyUID( DicomMask *mask , Study *study );

    /** Comprova que els dos PatientId el de la màscara i el de l'estudi facin matching. Si el Patient Id de la màscara està buit, per defecte retorna cert. En aquest cas fem wildcard matching
     */
    bool matchDicomMaskToPatientId( DicomMask *mask , Patient *patient );

    /** Comprova que la data de la màscara i la de l'estudi facin matching. Si la studyMaskDate és buida retorna cert per defecte
     */
    bool matchDicomMaskToStudyDate( DicomMask *mask , Study *study );

    /** Comprova que el nom del pacient de la màscara i el de l'estudi facin matching. Si la studyMaskPatientName és buida retorna cert per defecte. En aquest cas fem wildcard matching
     */
    bool matchDicomMaskToPatientName( DicomMask *mask , Patient *patient );

    ///A partir d'un DcmDirectoryRecord retorna les dades d'un Pacient
    Patient* fillPatient( DcmDirectoryRecord *dcmDirectoryRecordPatient );

    ///A partir d'un DcmDirectoryRecord retorna les dades d'un Study
    Study* fillStudy( DcmDirectoryRecord *dcmDirectoryRecordStudy );

    ///A partir d'un DcmDirectoryRecord retorna les dades d'un Series
    Series* fillSeries( DcmDirectoryRecord *dcmDirectoryRecordSeries );

    ///A partir d'un DcmDirectoryRecord retorna les dades d'un Image
    Image* fillImage( DcmDirectoryRecord *dcmDirectoryRecordImage );

    /** canvia les '\' per '/'. Això es degut a que les dcmtk retornen el path de la imatge en format Windows amb els directoris separats per '\'. En el cas de linux les hem de passar a '/'
     * TODO aquest mètode es pot substituir per QDir::toNativeSeparators() o similar que retornarà els separadors adequats al sistema
     * @param original path original
     * @return path amb '/'
     */
    QString backSlashToSlash( const QString &original );

    /**Ens construeix el Path relatiu d'una imatge, posa les '/' correctament i posa en minúscules o majúscules el nom del fitxer en funció de si el dicomdir conté els fitxers en minúscula o majúscula*/
    QString buildImageRelativePath( const QString &relativePath );
};

}

#endif
