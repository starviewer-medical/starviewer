#ifndef STUDY
#define STUDY

#include <string>
#include <list>

/** This class sets and gets study's fields
 */

namespace udg{
class Study
{

 public :
    
    ///constructor de la classe
    Study();
    
    /** Operador de la classe
     * @param  Estudi amb el que sl'ha de comprar
     * @return cert si és menor
     */
    bool operator<(Study);
    
    /** This action set the Patient's name.
     * @return  Patient's Name 
     */
    void setPatientName( std::string );
    
    /** This action set the Patient's birth date.
     * @param Patients birth date
     */
    void setPatientBirthDate( std::string );
    
    /** This action set the Patients Id.  
     * @param Patients Id.  
     */
    void setPatientId( std::string );
    
    /** This action set the Patients Sex .
     * @param Patients Sex  
     */
    void setPatientSex( std::string );
    
    /** This action set the Patient's Age .
     * @param Patients Age  
     */
    void setPatientAge( std::string ); 
    
    /** This action set the Study Id          
     * @param Study Id
     */
    void setStudyId( std::string );
    
    /** This action set the Study date        
     * @param Study date
     */
    void setStudyDate( std::string );
    
    /** This action set the Study Description 
     * @param Study Description
     */
    void setStudyDescription( std::string );

    /** This action set the Study Modality    
     * @param Study modality
     */
    void setStudyModality( std::string );
    
    /** This action set the Study time 
     * @param Study time
     */    
    void setStudyTime( std::string );
    
    /** This action set the Study UID    
     * @param Study UID
     */
    void setStudyUID( std::string );
    
    /** This action set the Institution Name    
     * @param  Institution Name
     */
    void setInstitutionName( std::string );
    
    /** This action set the Acession Number    
     * @param  Accession Number
     */
    void setAccessionNumber( std::string );
    
    /** This action set Pacs AE Title
     * @param CAlled PACS AE Title
     */
    void setPacsAETitle( std::string );
    
    /** Set the absolute path of the study
     * @param path de l'estudi
     */
    void setAbsPath( std::string );
    
    /** This function get the Patient's name.
     *              @return  Patient's Name 
     */
    std::string getPatientName();
    
    /** This function get the Patient's Birth Date
        *              @return  Patient's Birth Date
     */
    std::string getPatientBirthDate();
    
    /** This function get the Patient's Id   
    *              @return  Patient's Id   
    */
    std::string getPatientId();
    
    /** This function get the Patient's Sex. 
     *              @return  Patient's Sex 
     */
    std::string getPatientSex();
    
    /** This function get the Patient's Age. 
     *              @return  Patient's Age 
     */
    std::string getPatientAge();
    
    /** This function get the Study Id.  
     *              @return  Study's Id
     */
    std::string getStudyId();
    
    /** This function get the Study Date.
     *              @return  Study's Date
     */
    std::string getStudyDate();
    
    /** This function get the Study Time.
     *              @return  Study's Time
     */
    std::string getStudyTime();
    
    /** This function get the Study Description
     *              @return  Study's Description
     */
    std::string getStudyDescription();
    
    /** This function get the Study Modality.
     *              @return  Study's Modality
     */
    std::string getStudyModality();
    
    /** This function get the Institution name of the study
     *              @return  Institution name
     */
    std::string getInstitutionName();
    
    /** This function get the Study UID.
     *              @return  Study's UID
     */
    std::string getStudyUID();
    
    /** This function get the Accession name of the study
     *              @return  Accession name
     */
    std::string getAccessionNumber();
    
    /** This function get the AETitels of the study's PACS
     *              @return  AETitle
     */
    std::string getPacsAETitle();
    
    /** This function get the absolut path of the study, if the study is in the local disk
     *              @return  absolut path of the study,
     */
    std::string getAbsPath();
    
 private :
    
    std::string m_patientName;
    std::string m_patientBirthDate;
    std::string m_patientId;
    std::string m_patientSex;
    std::string m_patientAge;
    
    std::string m_studyId;
    std::string m_studyDate;
    std::string m_studyTime;
    std::string m_studyDescription;
    std::string m_studyModality;
    std::string m_studyUID;
    std::string m_accessionNumber;
    
    std::string m_institutionName;
    
    std::string m_pacsAETitle;
    std::string m_absPath;
};
}; //end namespace
#endif

