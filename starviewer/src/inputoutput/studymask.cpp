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
  *              @param   patientID Patient's ID of the patient to search. 
  *              @return state of the method
  */
Status StudyMask:: setPatientId( std::string patientID )
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_PatientID);
    
    elem->putString( patientID.c_str() );
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
  *              @param Name of the patient to search.
  *              @return state of the method
  */
Status StudyMask:: setPatientName( std::string patientName )
{
    Status state;
        
    //We spicified that we will use de PatientsName Tag
    DcmElement *elem = newDicomElement(DCM_PatientsName);
    
    elem->putString( patientName.c_str() );
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

/** Aquest mètode especifica la data de naixement del pacient amb la que s'han de cercar els estudis. El format és DDMMYYYY
  * Si el paràmetre no té valor, busca per totes les dates
  * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
  * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
  * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
  * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
  *              @param  Study's Data de naixement del pacient
  *              @return state of the method 
  */
Status StudyMask:: setPatientBirth( std::string date )
{
    Status state;
    
    //We specified that we will use the tag Patient Birth Date
    DcmElement *elem = newDicomElement(DCM_PatientsBirthDate);
    
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus(error_MaskLengthDate);
     
    elem->putString( date.c_str() );
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


/** This action especified that in the search we want the Patient's sex
  *              @param Patient's sex of the patient to search. 
  *              @return state of the method
  */
Status StudyMask:: setPatientSex( std::string patientSex )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_PatientsSex);
    
    elem->putString( patientSex.c_str() );
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
  *              @param  Patient's age of the patient to search. 
  *              @return state of the method
  */
Status StudyMask:: setPatientAge( std::string patientAge )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_PatientsAge);
    
    elem->putString( patientAge.c_str() );
    
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
  *              @param  Study's Id of the study to search.
  *              @return state of the method
  */
Status StudyMask:: setStudyId( std::string studyID )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyID);
    
    elem->putString( studyID.c_str() );
    
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

/** Aquest mètode especifica la data amb la que s'han de cercar els estudis. El format és DDMMYYYY
  * Si el paràmetre no té valor, busca per totes les dates
  * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
  * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
  * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
  * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
  *              @param  Study's date of the study to search.
  *              @return state of the method 
  */
Status StudyMask:: setStudyDate( std::string date )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyDate);
    
    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters) 
    if (date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus(error_MaskLengthDate);
    
    elem->putString( date.c_str() );
    
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
  *              @param Study's description of the study to search. 
  *              @return state of the method 
  */
Status StudyMask:: setStudyDescription( std::string desc )
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_StudyDescription);

    elem->putString( desc.c_str() );
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
  *              @param Study's modality the study to search.
  *              @return state of the method
  */
Status StudyMask:: setStudyModality( std::string modality )
{

    Status state;
    
    DcmElement *elem = newDicomElement(DCM_ModalitiesInStudy);
    
    elem->putString( modality.c_str() );
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

/** Especifica l'hora de l'estudi a buscar, les hores es passen en format HHMM
  *         Si es buit busca per qualsevol hora.
  *         Si se li passa una hora com 1753, buscarà series d'aquella hora
  *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
  *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
  *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores 
  *              @param  Hora de l'estudi
  *              @retun estat del mètode
  */
Status StudyMask:: setStudyTime( std::string time )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyTime);
    
    //la hora ha de ser de longitud 4 HHMM, o 5 HHMM- o -HHMM, o 9 HHMM-HHMM
    if ( time.length() != 4 && time.length() != 5 && time.length() != 9 && time.length() != 0 ) return state.setStatus(error_MaskLengthTime);
    
    elem->putString( time.c_str() );
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

/** This action especified that in the search we want the Study's instance UID
  *              @param Study's instance UID the study to search. 
  *              @return state of the method
  */
Status StudyMask:: setStudyUID( std::string studyUID )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);

    elem->putString( studyUID.c_str() );
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
  *              @param Institution name of the study to search. 
  *              @return state of the method
  */
Status StudyMask:: setInstitutionName( std::string institution )
{
    Status state;
    
    DcmElement *elem = newDicomElement(DCM_InstitutionName );

    elem->putString( institution.c_str() );
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
  *              @param Accession Number of the study to search. 
  *             @return state of the method
  */

Status StudyMask:: setAccessionNumber( std::string accession )
{
    char val[100];
    val[0] = '\0';
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_AccessionNumber );

    elem->putString( accession.c_str() );
    
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

