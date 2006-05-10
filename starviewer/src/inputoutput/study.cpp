#include "study.h"
#include <iostream>
#include <list>

namespace udg{

Study::Study()
{

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

std::string Study::getAbsPath()
{
    return m_absPath;
}

}

