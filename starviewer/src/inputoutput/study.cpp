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

void Study:: setPatientName( QString name )
{
    m_patientName = name;
}

void Study::setPatientBirthDate( QString date )
{
    m_patientBirthDate = date;
}

void Study::setPatientId( QString id )
{
    m_patientId = id;
}

void Study::setPatientSex( QString sex )
{
    m_patientSex = sex;
}

void Study::setPatientAge( QString age )
{
    m_patientAge = age;
}

void Study::setStudyId( QString id )
{
    m_studyId = id;
}

void Study::setStudyDate( QString date )
{
    m_studyDate = date;
}

void Study::setStudyDescription( QString desc )
{
    m_studyDescription = desc;
}

void Study::setStudyTime( QString time )
{
    m_studyTime = time;
}

void Study::setStudyModality( QString modality )
{
    m_studyModality = modality;
}

void Study::setStudyUID( QString uid )
{
    m_studyUID = uid;
}

void Study::setInstitutionName( QString institution )
{
    m_studyModality = institution;
}

void Study::setAccessionNumber( QString accession )
{
    m_accessionNumber = accession;
}

void Study::setPacsAETitle( QString title )
{
    m_pacsAETitle = title;
}

void Study::setAbsPath( QString path )
{
    m_absPath = path;
}

void Study::setReferringPhysiciansName( QString referringPhysiciansName )
{
    m_referringPhysiciansName = referringPhysiciansName;
}

/**********************************************************************************************************************/
/*                                    GET STUDY FIELDS                                                                */
/**********************************************************************************************************************/

QString Study:: getPatientName()
{
    return m_patientName;
}

QString Study::getPatientBirthDate()
{
    return m_patientBirthDate;
}

QString Study::getPatientId()
{
    return m_patientId;
}

QString Study::getPatientSex()
{
    return m_patientSex;
}

QString Study::getPatientAge()
{
    return m_patientAge;
}

QString Study::getStudyId()
{
    return m_studyId;
}

QString Study::getStudyDate()
{
    return m_studyDate;
}

QString Study::getStudyTime()
{
    return m_studyTime;
}

QString Study::getStudyDescription()
{
    return m_studyDescription;
}

QString Study::getStudyModality()
{
    return m_studyModality;
}

QString Study::getStudyUID()
{
    return m_studyUID;
}

QString Study::getInstitutionName()
{
    return m_institutionName;
}

QString Study::getAccessionNumber()
{
    return m_accessionNumber;
}

QString Study::getPacsAETitle()
{
    return m_pacsAETitle;
}

QString Study::getReferringPhysiciansName()
{
    return m_referringPhysiciansName;
}

QString Study::getAbsPath()
{
    return m_absPath;
}

}

