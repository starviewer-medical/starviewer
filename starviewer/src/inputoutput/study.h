#ifndef STUDY
#define STUDY

#include <string>
#include <list>

/** This class sets and gets study's fields
 */

namespace udg{
class Study{

 public :
    
    Study();
    
    bool operator<(Study);
    
    void setPatientName(std::string);
    void setPatientBirthDate(std::string);
    void setPatientId(std::string);
    void setPatientSex(std::string);
    void setPatientAge(std::string); 
    void setStudyId(std::string );
    void setStudyDate(std::string );
    //void setStudyTime(char* );
    void setStudyDescription(std::string);
    void setStudyModality(std::string);
    void setStudyTime(std::string);
    void setStudyUID(std::string);
    void setInstitutionName(std::string);
    void setAccessionNumber(std::string);
    void setPacsAETitle(std::string);
    void setAbsPath(std::string);
    
    std::string getPatientName( );
    std::string getPatientBirthDate( );
    std::string getPatientId( );
    std::string getPatientSex( );
    std::string getPatientAge( );
    std::string getStudyId( );
    std::string getStudyDate( );
    std::string getStudyTime( );
    std::string getStudyDescription( );
    std::string getStudyModality( );
    std::string getInstitutionName( );
    std::string getStudyUID( );
    std::string getAccessionNumber( );
    std::string getPacsAETitle( );
    std::string getAbsPath( );
    
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
};
#endif

