#ifndef DICOMSTUDY_H
#define DICOMSTUDY_H

#define HAVE_CONFIG_H 1

/** This class sets and gets study's fields
 */

#include <QString>

class DcmDataset;

namespace udg{



class DICOMStudy{
public:

    ///constructor de la classe
    DICOMStudy();

    /** Constructor de la classe a partir d'un objecte DcmDataset de les dcmtk
     * @param Objecte DcmDataset a partir del qual construir l'estudi
     */
    DICOMStudy( DcmDataset * );

    /** Operador de la classe
     * @param  Estudi amb el que sl'ha de comprar
     * @return cert si és menor
     */
    bool operator<(DICOMStudy);

    /** This action set the Patient's name.
     * @return  Patient's Name
     */
    void setPatientName( QString );

    /** This action set the Patient's birth date.
     * @param Patients birth date
     */
    void setPatientBirthDate( QString );

    /** This action set the Patients Id.
     * @param Patients Id.
     */
    void setPatientId( QString );

    /** This action set the Patients Sex .
     * @param Patients Sex
     */
    void setPatientSex( QString );

    /** This action set the Patient's Age .
     * @param Patients Age
     */
    void setPatientAge( QString );

    /** This action set the DICOMStudy Id
     * @param DICOMStudy Id
     */
    void setStudyId( QString );

    /** This action set the DICOMStudy date
     * @param DICOMStudy date
     */
    void setStudyDate( QString );

    /** This action set the DICOMStudy Description
     * @param DICOMStudy Description
     */
    void setStudyDescription( QString );

    /** This action set the DICOMStudy Modality
     * @param DICOMStudy modality
     */
    void setStudyModality( QString );

    /** This action set the DICOMStudy time
     * @param DICOMStudy time
     */
    void setStudyTime( QString );

    /** This action set the DICOMStudy UID
     * @param DICOMStudy UID
     */
    void setStudyUID( QString );

    /** This action set the Institution Name
     * @param  Institution Name
     */
    void setInstitutionName( QString );

    /** This action set the Acession Number
     * @param  Accession Number
     */
    void setAccessionNumber( QString );

    /** This action set Pacs AE Title
     * @param CAlled PACS AE Title
     */
    void setPacsAETitle( QString );

    /** Set the absolute path of the study
     * @param path de l'estudi
     */
    void setAbsPath( QString );

    /** estableix el referringPhysiciansName
     * @param  referringPhysiciansName
     */
    void setReferringPhysiciansName( QString referringPhysiciansName );

    /** This function get the Patient's name.
     *              @return  Patient's Name
     */
    QString getPatientName();

    /** This function get the Patient's Birth Date
        *              @return  Patient's Birth Date
     */
    QString getPatientBirthDate();

    /** This function get the Patient's Id
    *              @return  Patient's Id
    */
    QString getPatientId();

    /** This function get the Patient's Sex.
     *              @return  Patient's Sex
     */
    QString getPatientSex();

    /** This function get the Patient's Age.
     *              @return  Patient's Age
     */
    QString getPatientAge();

    /** This function get the DICOMStudy Id.
     *              @return  DICOMStudy's Id
     */
    QString getStudyId();

    /** This function get the DICOMStudy Date.
     *              @return  DICOMStudy's Date
     */
    QString getStudyDate();

    /** This function get the DICOMStudy Time.
     *              @return  DICOMStudy's Time
     */
    QString getStudyTime();

    /** This function get the DICOMStudy Description
     *              @return  DICOMStudy's Description
     */
    QString getStudyDescription();

    /** This function get the DICOMStudy Modality.
     *              @return  DICOMStudy's Modality
     */
    QString getStudyModality();

    /** This function get the Institution name of the study
     *              @return  Institution name
     */
    QString getInstitutionName();

    /** This function get the DICOMStudy UID.
     *              @return  DICOMStudy's UID
     */
    QString getStudyUID();

    /** This function get the Accession name of the study
     *              @return  Accession name
     */
    QString getAccessionNumber();

    /** This function get the AETitels of the study's PACS
     *              @return  AETitle
     */
    QString getPacsAETitle();

    /** This function get the absolut path of the study, if the study is in the local disk
     *              @return  absolut path of the study,
     */
    QString getAbsPath();

    /** retorna el referringPhysiciansName
     * @return referringPhysiciansName
     */
    QString getReferringPhysiciansName();

 private :

    QString m_patientName;
    QString m_patientBirthDate;
    QString m_patientId;
    QString m_patientSex;
    QString m_patientAge;

    QString m_studyId;
    QString m_studyDate;
    QString m_studyTime;
    QString m_studyDescription;
    QString m_studyModality;
    QString m_studyUID;
    QString m_accessionNumber;

    QString m_institutionName;

    QString m_pacsAETitle;
    QString m_absPath;
    QString m_referringPhysiciansName;
};
}; //end namespace
#endif

