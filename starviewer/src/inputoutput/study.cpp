#include "study.h"

namespace udg{

Study::Study()
{

}

Study::Study( DcmDataset * studyDcmDataset )
{
    const char *text;

    studyDcmDataset->findAndGetString( DCM_PatientsName , text , false );
    if ( text != NULL ) setPatientName( text );

    //set Patient's Birth Date
    studyDcmDataset->findAndGetString( DCM_PatientsBirthDate , text , false );
    if ( text != NULL ) setPatientBirthDate( text );

    //set Patient's Id
    studyDcmDataset->findAndGetString( DCM_PatientID , text , false );
    if ( text != NULL ) setPatientId( text );

    //set Patient's Sex
    studyDcmDataset->findAndGetString( DCM_PatientsSex , text , false );
    if ( text != NULL ) setPatientSex( text );

    //set Patiens Age
    studyDcmDataset->findAndGetString( DCM_PatientsAge , text , false );
    if ( text != NULL ) setPatientAge( text );

    //set Study ID
    studyDcmDataset->findAndGetString( DCM_StudyID , text , false );
    if ( text != NULL ) setStudyId( text );

    //set Study Date
    studyDcmDataset->findAndGetString( DCM_StudyDate , text , false );
    if ( text != NULL ) setStudyDate( text );

        //set Study Description
    studyDcmDataset->findAndGetString( DCM_StudyDescription , text , false );
    if ( text != NULL ) setStudyDescription( text );

    //set Study Time
    studyDcmDataset->findAndGetString( DCM_StudyTime , text , false );
    if ( text != NULL ) setStudyTime( text );

    //set Institution Name
    studyDcmDataset->findAndGetString( DCM_InstitutionName , text , false );
    if ( text != NULL ) setInstitutionName( text );

    //set StudyUID
    studyDcmDataset->findAndGetString( DCM_StudyInstanceUID , text , false );
    if ( text != NULL ) setStudyUID( text );

    //set Accession Number
    studyDcmDataset->findAndGetString( DCM_AccessionNumber , text , false );
    if ( text != NULL ) setAccessionNumber( text );

    //set Study Modality
    studyDcmDataset->findAndGetString( DCM_ModalitiesInStudy, text , false );
    if ( text != NULL ) setStudyModality( text );

    //set PACS AE Title Called
    studyDcmDataset->findAndGetString( DCM_RetrieveAETitle , text , false );
    if ( text != NULL ) setPacsAETitle( text );

    studyDcmDataset->findAndGetString( DCM_ReferringPhysiciansName , text , false );
    if ( text != NULL ) setReferringPhysiciansName( text );
}

bool Study::operator < ( Study a )
{
    if ( m_patientName<a.getPatientName() )
    {
        return true;
    }
    else return false;
}

/**********************************************************************************************************************/
/*                                    SET STUDY FIELDS                                                               */
/********************************************************************************************************************/

void Study:: setPatientName( std::string name)
{
    m_patientName.erase();
    m_patientName.insert( 0 , name );
}

void Study::setPatientBirthDate( std::string date )
{
    m_patientBirthDate.erase();
    m_patientBirthDate.insert( 0 , date );
}

void Study::setPatientId( std::string id )
{
    m_patientId.erase();
    m_patientId.insert( 0 , id );
}

void Study::setPatientSex( std::string sex )
{
    m_patientSex.erase();
    m_patientSex.insert( 0 , sex );
}

void Study::setPatientAge( std::string age )
{
    m_patientAge.erase();
    m_patientAge.insert( 0 , age );
}

void Study::setStudyId( std::string id )
{
    m_studyId.erase();
    m_studyId.insert( 0 , id );
}

void Study::setStudyDate( std::string date )
{
    m_studyDate.erase();
    m_studyDate.insert( 0 , date );
}

void Study::setStudyDescription( std::string desc )
{
    m_studyDescription.erase();
    m_studyDescription.insert( 0 , desc );
}

void Study::setStudyTime( std::string time )
{
    m_studyTime.erase();
    m_studyTime.insert( 0 , time );
}

void Study::setStudyModality( std::string modality )
{
    m_studyModality.erase();
    m_studyModality.insert( 0 , modality );
}

void Study::setStudyUID( std::string uid )
{
    m_studyUID.erase();
    m_studyUID.insert( 0 , uid );
}

void Study::setInstitutionName( std::string institution )
{
    m_studyModality.erase();
    m_studyModality.insert( 0 , institution );
}

void Study::setAccessionNumber( std::string accession )
{
    m_accessionNumber.erase();
    m_accessionNumber.insert( 0 , accession );
}

void Study::setPacsAETitle( std::string title )
{
    m_pacsAETitle.erase();
    m_pacsAETitle.insert( 0 , title );
}

void Study::setAbsPath( std::string path )
{
    m_absPath.erase();
    m_absPath.insert( 0 , path );
}

void Study::setReferringPhysiciansName( std::string referringPhysiciansName )
{
    m_referringPhysiciansName.erase();
    m_referringPhysiciansName.insert( 0 , referringPhysiciansName );
}

/**********************************************************************************************************************/
/*                                    GET STUDY FIELDS                                                                */
/**********************************************************************************************************************/

std::string Study:: getPatientName()
{
    return m_patientName;
}

std::string Study::getPatientBirthDate()
{
    return m_patientBirthDate;
}

std::string Study::getPatientId()
{
    return m_patientId;
}

std::string Study::getPatientSex()
{
    return m_patientSex;
}

std::string Study::getPatientAge()
{
    return m_patientAge;
}

std::string Study::getStudyId()
{
    return m_studyId;
}

std::string Study::getStudyDate()
{
    return m_studyDate;
}

std::string Study::getStudyTime()
{
    return m_studyTime;
}

std::string Study::getStudyDescription()
{
    return m_studyDescription;
}

std::string Study::getStudyModality()
{
    return m_studyModality;
}

std::string Study::getStudyUID()
{
    return m_studyUID;
}

std::string Study::getInstitutionName()
{
    return m_institutionName;
}

std::string Study::getAccessionNumber()
{
    return m_accessionNumber;
}

std::string Study::getPacsAETitle()
{
    return m_pacsAETitle;
}

std::string Study::getReferringPhysiciansName()
{
    return m_referringPhysiciansName;
}

std::string Study::getAbsPath()
{
    return m_absPath;
}

}

