#include "studymask.h"

namespace udg{

StudyMask::StudyMask()
{
    m_mask = new DcmDataset;
    retrieveLevel();
}

/** This action especified that the query search, will use the retrieve level Study. For any doubts about this retrieve level and the query/retrieve fields,
 consult DICOMS's documentation in Chapter 4, C.6.2.1
*/
void StudyMask:: retrieveLevel()
{
    char val[15];
   
    DcmElement *elem = newDicomElement(DCM_QueryRetrieveLevel);
    
    strcpy(val,"STUDY");
    elem->putString(val);

    m_mask->insert(elem, OFTrue);
}



/************************* PATIENT  *********************************************************************/

/** This action especified that in the search we want the Patient's ID
  *              @param   patientID Patient's ID of the patient to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setPatientId(std::string patientID )
{
    char val[70];
    val[0] = '\0';
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_PatientID);
    
    //if the patientId is null we supose that the user don't apply a criterium in this field
    if (patientID.length()==0)
    {
        value = "";
    }
    else if (patientID.length()>0)
    {
        value = patientID.c_str();
    }
    else value = "";
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientId);     
    }

    //insert the tag PATIENT ID in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskPatientId);
    }
   
    return state.setStatus(correct);
}

/** This action especified that in the search we want the Patient's Name
  *              @param Name of the patient to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setPatientName(std::string patientName )
{
    char val[70];
    val[0] = '\0';
    std::string value;
    Status state;
        
    //We spicified that we will use de PatientsName Tag
    DcmElement *elem = newDicomElement(DCM_PatientsName);
    
    //if the patientId is null we supose that the user don't apply a criterium in this field
    
    if (patientName.length() == 0)
    {
         value = "";
    }
    else if (patientName.length()>0)
    {
        value =  patientName;
    }
    else value = "";
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientName);
    }

    //insert the tag PATIENT NAME in the search mask
    m_mask->insert(elem, OFTrue);  
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskPatientName);
    }
    return state.setStatus(correct);
}

/** This action especified that in the search we want the Patient's Birth
  *               @param Date of birth of the patient to search. If this parameter is null it's supose that any mask is applied at this field. Date's format is YYYYMMDD
  **              @return state of the method
  */
Status StudyMask:: setPatientBirth(std::string date)
{
    char val[10];
    val[0] = '\0';
    std::string value;
    Status state;
    
    //We specified that we will use the tag Patient Birth Date
    DcmElement *elem = newDicomElement(DCM_PatientsBirthDate);
    
    if (date.length() == 0)
    {
       value = ""; 
    }
    else if (date.length() >0)
    {
        value = date;
    }
    else return state.setStatus(error_MaskLengthDate);
     
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientBirth);
    }

    //insert the tag PATIENT BIRTH in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskPatientBirth);
    }
    
    return state.setStatus(correct);
}

/** This action especified that in the search we want select patient who has born between these two dates. Time's format is YYYYMMDD
  *              @param Min Patient's Bitrh  to search.  
  *              @param Max Patient's  to search.
  *              @return state of the method
  */
Status StudyMask:: setPatientBirth(std::string dateMin,std::string dateMax)
{
    char val[20];
    val[0] = '\0';
    std::string value;
    Status state;
    
    //The length of the date must be 8 numbers. The correct Format is YYYYMMDD
    if (dateMin.length()!=8 || dateMax.length()!=8) 
    {
        return state.setStatus(error_MaskLengthDate);
    } 
    
    //We specified that we will use the tag Patient Birth Date
    DcmElement *elem = newDicomElement(DCM_PatientsBirthDate);
     
    value = dateMin;
    value.append("-");
    value.append(dateMax);      
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientBirth);
    }


    //insert the tag PATIENT BIRTH in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) 
    {
        return state.setStatus(errorMaskPatientBirth);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Patient's sex
  *              @param Patient's sex of the patient to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setPatientSex(std::string patientSex )
{
    char val[3];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_PatientsSex);
    
    //if the patientId is null we supose that the user don't apply a criterium in this field
    if (patientSex.length()==0)
    {
        value = "";
    }
    else if (patientSex.length()>0)
    {
        value = patientSex;
    }
    else value = "";
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientSex);
    }

    //insert the tag PATIENT SEX in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskPatientSex);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Patient's Age
  *              @param  Patient's age of the patient to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setPatientAge(std::string patientAge )
{
    char val[3];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_PatientsAge);
    
    //if the patientAge is null we supose that the user don't apply a criterium in this field
    if (patientAge.length()==0)
    {
        value = "";
    }
    else if (patientAge.length()>0)
    {
        value = patientAge;
    }
    else value = "";
    
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskPatientAge);
    }

    //insert the tag PATIENT Age in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskPatientAge);
    }

    return state.setStatus(correct);
}
/************************************************************ STUDY MASK********************************************************************************/

/** This action especified that in the search we want the Study's Id
  *              @param  Study's Id of the study to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setStudyId(std::string studyID )
{
    char val[20];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyID);
    
    //if the StudyId is null we supose that the user don't apply a criterium in this field
    if (studyID.length()==0)
    {
        value = "";
    }
    else if (studyID.length()>0)
    {
       value= studyID;
    }
    else value = "";
    
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskStudyId);
    }
    
    //insert the tag STUDY DATE in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskStudyId);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's date. Date's format is YYYYMMDD
  *              @param  Study's date of the study to search. If this parameter is null it's supose that any mask is applied at this field.
  *              @return state of the method 
  */
Status StudyMask:: setStudyDate(std::string date )
{
    char val[10];
    val[0] = '\0';
    std::string value;    
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyDate);
    
    //if the Date is null we supose that the user don't apply a criterium in this field
    if (date.length()==0)
    {
        value = "";
    }
    else if (date.length()>0)
    {
        value = date;
    }
    else return state.setStatus(error_MaskLengthDate);
    
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
       return state.setStatus(errorMaskStudyDate);
    }

    //insert the tag STUDY DATE in the search mask    
    if (m_mask == NULL) m_mask = new DcmDataset;
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskStudyDate);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's time. Time's format is HHMM
  *              @param Min date Study of the study to search.  
  *              @param Max date Study of the study to search.
  *              @return state of the method
  */
Status StudyMask:: setStudyDate(std::string dateMin,std::string dateMax )
{
    char val[20];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyDate);

    //the length of the date must be 8 numbers. The correct format is YYYYMMDD   
    if (dateMin.length()!=8 || dateMax.length()!=8) 
    {
        return state.setStatus(error_MaskLengthDate);
    }
     
    value =dateMin;
    value.append("-");
    value.append(dateMax);
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskStudyDate);
    }
    
    //insert the tag STUDY DATE in the search mask    
    if (m_mask == NULL) m_mask = new DcmDataset;
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskStudyDate);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's description
  *              @param Study's description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
  *              @return state of the method 
  */
Status StudyMask:: setStudyDescription(std::string desc)
{
    char val[70];
    val[0] = '\0';
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_StudyDescription);

    //if the desc is null we supose that the user don't apply a criterium in this field
    if (desc.length()==0)
    {
        value = "";
    }
    else if (desc.length()>0)
    {
        value = desc;
    }
    else value = "";

    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskStudyDescription);
    }
    
    //insert the tag STUDY DESCRIPTION in the search mask    
    m_mask->insert(elem, OFTrue);
        
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(errorMaskStudyDescription);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's modality
  *              @param Study's modality the study to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setStudyModality(std::string modality)
{
    char val[50];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_ModalitiesInStudy);

    //if the modality is null we supose that the user don't apply a criterium in this field
    if (modality.length()==0)
    {
        value = "";
    }
    else if (modality.length()>0)
    {
        value = modality;
    }
    else value = "";
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskStudyModality);
    }
    
    //insert the tag STUDY Modality in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(error_MaskStudyModality);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's time
  *              @param Study's time the study to search. If this parameter is null it's supose that any mask is applied at this field. Time's format is HHMM
  *              @return state of the method
  */
Status StudyMask:: setStudyTime(std::string time)
{
    char val[6];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyTime);
    
    //if the time is null we supose that the user don't apply a criterium in this field
    if (time.length()==0)
    {
        value = "";
    }
    else if (time.length()==4)
    {
        value = time;
    }
    else return state.setStatus(error_MaskLengthTime);
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskStudyTime);
    }
    
    //insert the tag STUDY TIME in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) 
    {
        return state.setStatus(error_MaskStudyTime);
    }
       
    return state.setStatus(correct);
}

/** This action especified that in the search we want to select Studies realized between these two times. Time's format is HHMM
  *              @param Min time  Study of the study to search.  
  *              @param Max time Study of the study to search.
  *              @return state of the method
  */
Status StudyMask:: setStudyTime(std::string timeMin,std::string timeMax)
{
    char val[14];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyTime);

    //the length of the time must be 4 numbers. The correct format is YYYYMMDD
    if (timeMin.length()!=4 || timeMax.length()!=4) 
    {
        return state.setStatus(error_MaskLengthTime);
    }
     
    value = timeMin;
    value.append("-");
    value.append(timeMax);
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskStudyTime);
    }
    

    //insert the tag STUDY TIME in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(error_MaskStudyTime);
    }
       
    return state.setStatus(correct);
}

/** This action especified that in the search we want the Study's instance UID
  *              @param Study's instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setStudyUID(std::string uid)
{
    char val[70];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);

    //if the time is null we supose that the user don't apply a criterium in this field
    if (uid.length()==0)
    {
        value = "";
    }
    else if (uid.length()>0)
    {
        value = uid;
    }
    else strcpy(val,"");
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskStudyUID);
    }
    
    //insert the tag STUDY UID in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(error_MaskStudyUID);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Institution name
  *              @param Institution name of the study to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return state of the method
  */
Status StudyMask:: setInstitutionName(std::string institution)
{
    char val[100];
    val[0] = '\0';
    std::string value;
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_InstitutionName );

    //if the time is null we supose that the user don't apply a criterium in this field
    if (institution.length()==0)
    {
        value = "";
    }
    else if (institution.length()>0)
    {
        value = institution;
    }
    else value = "";;
    
    elem->putString(value.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskInstitutionName);
    }

    //insert the tag Institution name in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(error_MaskInstitutionName);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the Accession Number
  *              @param Accession Number of the study to search. If this parameter is null it's supose that any mask is applied at this field
  **             @return state of the method
  */

Status StudyMask:: setAccessionNumber(std::string accession)
{
    char val[100];
    val[0] = '\0';
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_AccessionNumber );

    //if the time is null we supose that the user don't apply a criterium in this field
    if (accession.length()==0)
    {
        value = "";
    }
    else if (accession.length()>0)
    {
        value = accession;
    }
    else value = "";
    
    elem->putString(value.c_str());
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskAccessionNumber);
    }

    //insert the tag Accession Number in the search mask    
    m_mask->insert(elem, OFTrue);
    if (m_mask->error() != EC_Normal) {
        return state.setStatus(error_MaskAccessionNumber);
    }

    return state.setStatus(correct);
}

/***************************************************************   GET **********************************************/

/** Retorna de la màscara l'estudi UID
  *            @param mask [in] màscara de la cerca
  *            @return   Estudi UID que cerquem
  */
std::string StudyMask::getStudyUID()
{
    const char * UID=NULL;
    std::string studyUID;
    
    DcmTagKey studyUIDTagKey (DCM_StudyInstanceUID);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyUIDTagKey, UID, OFFalse );;
    
    if (UID != NULL) studyUID.insert(0,UID);
        
    return studyUID;
}

/** Retorna el Id del pacient a buscar
  *            @return   ID del pacient
  */
std::string StudyMask::getPatientId()
{
    const char * ID = NULL;
    std::string patientID;
    
    DcmTagKey patientIDTagKey (DCM_PatientID);
    OFCondition ec;
    ec = m_mask->findAndGetString( patientIDTagKey, ID, OFFalse );
    
    
    if (ID != NULL) patientID.insert(0,ID);
    
    return patientID;
}

/** Retorna el nom del pacient 
  *            @return Nom del pacient 
  */
std::string StudyMask::getPatientName()
{
    const char * name = NULL;
    std::string patientName;
    
    DcmTagKey patientIDTagKey (DCM_PatientsName);
    OFCondition ec;
    ec = m_mask->findAndGetString( patientIDTagKey, name, OFFalse );
    
    
    if (name != NULL) patientName.insert(0,name);
    
    return patientName;
}

/** Retorna la data naixement
  *            @return Data de naixament del pacient
  */
std::string StudyMask::getPatientBirth()
{
    const char * birth = NULL;
    std::string patientBirth;
    
    DcmTagKey patientBirthTagKey (DCM_PatientsBirthDate);
    OFCondition ec;
    ec = m_mask->findAndGetString( patientBirthTagKey, birth, OFFalse );
    
    
    if ( birth != NULL) patientBirth.insert(0,birth);
    
    return patientBirth;
}

/** Retorna el sexe del pacient
  *            @return sexe del pacient 
  */
std::string StudyMask::getPatientSex()
{
    const char * sex = NULL;
    std::string patientSex;
    
    DcmTagKey patientSexTagKey (DCM_PatientsSex);
    OFCondition ec;
    ec = m_mask->findAndGetString( patientSexTagKey, sex, OFFalse );
    
    if (sex != NULL) patientSex.insert(0,sex);
    
    return patientSex;
}

/** Retorna l'edat de pacient
  *            @return edat del pacient 
  */
std::string StudyMask::getPatientAge()
{
    const char * age = NULL;
    std::string patientAge;
    
    DcmTagKey patientAgeTagKey (DCM_PatientsAge);
    OFCondition ec;
    ec = m_mask->findAndGetString( patientAgeTagKey, age, OFFalse );
    
    if (age != NULL) patientAge.insert(0,age);
    
    return patientAge;
}

/** Retorna Id de l'estudi
  *            @return   ID de l'estudi
  */
std::string StudyMask::getStudyId()
{
    const char * ID = NULL;
    std::string studyID;
    
    DcmTagKey studyIDTagKey (DCM_StudyID);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyIDTagKey, ID, OFFalse );
    
    if (ID != NULL) studyID.insert(0,ID);
    
    return studyID;
}

/** Retorna la data de l'estudi
  *            @return   Data de l'estudi
  */
std::string StudyMask::getStudyDate()
{
    const char * date = NULL;
    std::string studyDate;
    
    DcmTagKey studyDateTagKey (DCM_StudyDate);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDateTagKey, date, OFFalse );
    
    if (date != NULL) studyDate.insert(0,date);
    
    return studyDate;
}

/** Retorna la descripció de l'estudi
  *            @return   descripció de l'estudi
  */
std::string StudyMask::getStudyDescription()
{
    const char * description = NULL;
    std::string studyDescription;
    
    DcmTagKey studyDescriptionTagKey (DCM_StudyDescription);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDescriptionTagKey, description, OFFalse );
    
    if (description != NULL) studyDescription.insert(0 , description);
    
    return studyDescription;
}

/** Retorna l'hora de l'estudi
  *            @return   hora de l'estudi 
  */
std::string StudyMask::getStudyTime()
{
    const char * time = NULL;
    std::string studyTime;
    
    DcmTagKey studyTimeTagKey (DCM_StudyTime);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyTimeTagKey, time, OFFalse );
    
    if (time != NULL) studyTime.insert(0,time);
    
    return studyTime;
}

/** Retorna de la modalitat de l'estudi 
  *            @return   Modalitat de l'estudi
  */
std::string StudyMask::getStudyModality()
{
    const char * mod = NULL;
    std::string studyModality;
    
    DcmTagKey studyModalityTagKey (DCM_ModalitiesInStudy);
    OFCondition ec;
    ec = m_mask->findAndGetString( studyModalityTagKey, mod, OFFalse );
    
    if (mod != NULL) studyModality.insert(0,mod);
    
    return studyModality;
}

/** Retorna el accession number de l'estudi 
  *            @return   accession number de l'estudi
  */
std::string StudyMask::getAccessionNumber()
{
    const char * aNumber = NULL;
    std::string accessionNumber;
    
    DcmTagKey accessionNumberTagKey (DCM_AccessionNumber);
    OFCondition ec;
    ec = m_mask->findAndGetString( accessionNumberTagKey, aNumber, OFFalse );
    
    if (aNumber != NULL) accessionNumber.insert(0,aNumber);
        
    return accessionNumber;
}

/** Retorna el nom de l'institució on s'ha realitzat l'estudi 
  *            @return   Nom de l'institucio
  */
std::string StudyMask::getInstitutionName()
{
    const char * institution = NULL;
    std::string institutionName;
    
    DcmTagKey institutionNameTagKey (DCM_InstitutionName);
    OFCondition ec;
    ec = m_mask->findAndGetString( institutionNameTagKey, institution, OFFalse );
    
    if (institution != NULL) institutionName.insert(0,institution);
        
    return institutionName;
}

/**  Return the generated search mask
              @return returns de search mask
*/
DcmDataset* StudyMask::getMask()
{
    return m_mask;
}

};

