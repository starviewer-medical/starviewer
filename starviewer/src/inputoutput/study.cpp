#include "study.h"
#include <iostream>
#include <list>


namespace udg{


/** Initialize the fields of the study
*/
Study::Study()
{

}

/** operator to sort the study list by patient name
  */
bool Study::operator<(Study a)
{
    if (m_patientName<a.getPatientName())
    {
        return true;
    }
    else return false;
}           

/*******************************************************************************************************************************/
/*                                    SET STUDY FIELDS                                                                         */
/*******************************************************************************************************************************/

/** This action set the Patient's name.
  *              @return  Patient's Name 
  */
void Study:: setPatientName(std::string name)
{
    m_patientName.erase();
    m_patientName.insert(0,name);
}

/** This action set the Patient's birth date.
  *              @param Patients birth date
  */
void Study::setPatientBirthDate(std::string date)
{
    m_patientBirthDate.erase();
    m_patientBirthDate.insert(0,date);
}

/** This action set the Patients Id.  
  *              @param Patients Id.  
  */
void Study::setPatientId(std::string id)
{
    m_patientId.erase();
    m_patientId.insert(0,id);
}

/** This action set the Patients Sex .
  *              @param Patients Sex  
  */
void Study::setPatientSex(std::string sex)
{
    m_patientSex.erase();
    m_patientSex.insert(0,sex);
}

/** This action set the Patient's Age .
  *              @param Patients Age  
  */
void Study::setPatientAge(std::string age)
{
    m_patientAge.erase();
    m_patientAge.insert(0,age);
}

/** This action set the Study Id          
  *              @param Study Id
  */
void Study::setStudyId(std::string id)
{
    m_studyId.erase();
    m_studyId.insert(0,id);
}

/** This action set the Study date        
  *              @param Study date
  */
void Study::setStudyDate(std::string date)
{
    m_studyDate.erase();
    m_studyDate.insert(0,date);
}
/** This action set the Study Description 
  *              @param Study Description
  */
void Study::setStudyDescription(std::string desc)
{
    m_studyDescription.erase();
    m_studyDescription.insert(0,desc);
}

/** This action set the Study time 
  *              @param Study time
  */
void Study::setStudyTime(std::string time)
{
    m_studyTime.erase();
    m_studyTime.insert(0,time);
}

/** This action set the Study Modality    
  *              @param Study modality
  */
void Study::setStudyModality(std::string modality)
{
    m_studyModality.erase();
    m_studyModality.insert(0,modality);
}

/** This action set the Study UID    
  *              @param Study UID
  */
void Study::setStudyUID(std::string uid)
{
    m_studyUID.erase();
    m_studyUID.insert(0,uid);
}

/** This action set the Institution Name    
  *              @param  Institution Name
  */
void Study::setInstitutionName(std::string institution)
{
    m_studyModality.erase();
    m_studyModality.insert(0,institution);
}

/** This action set the Acession Number    
  *              @param  Accession Number
  */
void Study::setAccessionNumber(std::string accession)
{
    m_accessionNumber.erase();
    m_accessionNumber.insert(0,accession);
}


/** This action set Pacs AE Title
  *              @param CAlled PACS AE Title
  */
void Study::setPacsAETitle(std::string title)
{
    m_pacsAETitle.erase();
    m_pacsAETitle.insert(0,title);
}

/** Set the absolute path of the study
  */
void Study::setAbsPath(std::string path)
{
    m_absPath.erase();
    m_absPath.insert(0,path);
}

/*******************************************************************************************************************************/
/*                                    GET STUDY FIELDS                                                                         */
/*******************************************************************************************************************************/


/** This function get the Patient's name.
  *              @return  Patient's Name 
  */
std::string Study:: getPatientName( )
{

    return m_patientName;
}

/** This function get the Patient's Birth Date
  *              @return  Patient's Birth Date
  */
std::string Study::getPatientBirthDate( )
{
    return m_patientBirthDate;
}

/** This function get the Patient's Id   
  *              @return  Patient's Id   
  */
std::string Study::getPatientId( )
{
    return m_patientId;
}

/** This function get the Patient's Sex. 
  *              @return  Patient's Sex 
  */
std::string Study::getPatientSex( )
{
    return m_patientSex;
}

/** This function get the Patient's Age. 
  *              @return  Patient's Age 
  */
std::string Study::getPatientAge( )
{
    return m_patientAge;
}

/** This function get the Study Id.  
  *              @return  Study's Id
  */
std::string Study::getStudyId( )
{
    return m_studyId;
}

/** This function get the Study Date.
  *              @return  Study's Date
  */
std::string Study::getStudyDate( )
{
    return m_studyDate;
}

/** This function get the Study Time.
  *              @return  Study's Time
  */
std::string Study::getStudyTime( )
{
    return m_studyTime;
}

/** This function get the Study Description
  *              @return  Study's Description
  */
std::string Study::getStudyDescription( )
{
    return m_studyDescription;
}

/** This function get the Study Modality.
  *              @return  Study's Modality
  */
std::string Study::getStudyModality( )
{
    return m_studyModality;
}

/** This function get the Study UID.
  *              @return  Study's UID
  */
std::string Study::getStudyUID( )
{
   // printf("aki %s\n",m_studyUID);
    return m_studyUID;
}


/** This function get the Institution name of the study
  *              @return  Institution name
  */
std::string Study::getInstitutionName( )
{
    return m_institutionName;
}

/** This function get the Accession name of the study
  *              @return  Accession name
  */
std::string Study::getAccessionNumber( )
{
    return m_accessionNumber;
}

/** This function get the Accession name of the study
  *              @return  Accession name
  */
std::string Study::getPacsAETitle( )
{
    return m_pacsAETitle;
}

/** This function get the absolut path of the study, if the study is in the local disk
  *              @return  absolut path of the study,
  */
std::string Study::getAbsPath( )
{
    return m_absPath;
}

}

