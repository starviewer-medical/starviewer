#include "dicomstudy.h"

#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags

namespace udg{

DICOMStudy::DICOMStudy()
{
}

DICOMStudy::DICOMStudy( DcmDataset * studyDcmDataset )
{
    const char *text;

    studyDcmDataset->findAndGetString( DCM_PatientsName , text , false );
    //TODO sabem que els estudis amb els que treballem tenen el Specific Character Set (0008,0005) el ISO_IR 100 que és el Latin 1, però que passa si tenim imatges que no tenen aquest tipus de Specific Character Set, ( El latin1 és el més usat a centre Europa). Consulta el C.12.1.1.2 del PS 3.3 per més informació i el PS 3.5
    if ( text != NULL ) setPatientName( QString::fromLatin1( text ) );

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

    //set DICOMStudy ID
    studyDcmDataset->findAndGetString( DCM_StudyID , text , false );
    if ( text != NULL ) setStudyId( text );

    //set DICOMStudy Date
    studyDcmDataset->findAndGetString( DCM_StudyDate , text , false );
    if ( text != NULL ) setStudyDate( text );

        //set DICOMStudy Description
    studyDcmDataset->findAndGetString( DCM_StudyDescription , text , false );
    if ( text != NULL ) setStudyDescription( QString::fromLatin1( text ) );

    //set DICOMStudy Time
    studyDcmDataset->findAndGetString( DCM_StudyTime , text , false );
    if ( text != NULL ) setStudyTime( text );

    //set Institution Name
    studyDcmDataset->findAndGetString( DCM_InstitutionName , text , false );
    if ( text != NULL ) setInstitutionName( QString::fromLatin1( text ) );

    //set DICOMStudyUID
    studyDcmDataset->findAndGetString( DCM_StudyInstanceUID , text , false );
    if ( text != NULL ) setStudyUID( text );

    //set Accession Number
    studyDcmDataset->findAndGetString( DCM_AccessionNumber , text , false );
    if ( text != NULL ) setAccessionNumber( text );

    //set DICOMStudy Modality
    studyDcmDataset->findAndGetString( DCM_ModalitiesInStudy, text , false );
    if ( text != NULL ) setStudyModality( text );

    //set PACS AE Title Called
    studyDcmDataset->findAndGetString( DCM_RetrieveAETitle , text , false );
    if ( text != NULL ) setPacsAETitle( text );

    studyDcmDataset->findAndGetString( DCM_ReferringPhysiciansName , text , false );
    if ( text != NULL ) setReferringPhysiciansName( QString::fromLatin1( text ) );
}

bool DICOMStudy::operator < ( DICOMStudy a )
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

void DICOMStudy:: setPatientName( QString name )
{
    m_patientName = name;
}

void DICOMStudy::setPatientBirthDate( QString date )
{
    m_patientBirthDate = date;
}

void DICOMStudy::setPatientId( QString id )
{
    m_patientId = id;
}

void DICOMStudy::setPatientSex( QString sex )
{
    m_patientSex = sex;
}

void DICOMStudy::setPatientAge( QString age )
{
    m_patientAge = age;
}

void DICOMStudy::setStudyId( QString id )
{
    m_studyId = id;
}

void DICOMStudy::setStudyDate( QString date )
{
    m_studyDate = date;
}

void DICOMStudy::setStudyDescription( QString desc )
{
    m_studyDescription = desc;
}

void DICOMStudy::setStudyTime( QString time )
{
    m_studyTime = time;
}

void DICOMStudy::setStudyModality( QString modality )
{
    m_studyModality = modality;
}

void DICOMStudy::setStudyUID( QString uid )
{
    m_studyUID = uid;
}

void DICOMStudy::setInstitutionName( QString institution )
{
    m_studyModality = institution;
}

void DICOMStudy::setAccessionNumber( QString accession )
{
    m_accessionNumber = accession;
}

void DICOMStudy::setPacsAETitle( QString title )
{
    m_pacsAETitle = title;
}

void DICOMStudy::setAbsPath( QString path )
{
    m_absPath = path;
}

void DICOMStudy::setReferringPhysiciansName( QString referringPhysiciansName )
{
    m_referringPhysiciansName = referringPhysiciansName;
}

/**********************************************************************************************************************/
/*                                    GET STUDY FIELDS                                                                */
/**********************************************************************************************************************/

QString DICOMStudy:: getPatientName()
{
    return m_patientName;
}

QString DICOMStudy::getPatientBirthDate()
{
    return m_patientBirthDate;
}

QString DICOMStudy::getPatientId()
{
    return m_patientId;
}

QString DICOMStudy::getPatientSex()
{
    return m_patientSex;
}

QString DICOMStudy::getPatientAge()
{
    return m_patientAge;
}

QString DICOMStudy::getStudyId()
{
    return m_studyId;
}

QString DICOMStudy::getStudyDate()
{
    return m_studyDate;
}

QString DICOMStudy::getStudyTime()
{
    return m_studyTime;
}

QString DICOMStudy::getStudyDescription()
{
    return m_studyDescription;
}

QString DICOMStudy::getStudyModality()
{
    return m_studyModality;
}

QString DICOMStudy::getStudyUID()
{
    return m_studyUID;
}

QString DICOMStudy::getInstitutionName()
{
    return m_institutionName;
}

QString DICOMStudy::getAccessionNumber()
{
    return m_accessionNumber;
}

QString DICOMStudy::getPacsAETitle()
{
    return m_pacsAETitle;
}

QString DICOMStudy::getReferringPhysiciansName()
{
    return m_referringPhysiciansName;
}

QString DICOMStudy::getAbsPath()
{
    return m_absPath;
}

}

