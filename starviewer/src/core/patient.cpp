/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patient.h"
#include "logging.h"

namespace udg {

Patient::Patient(QObject *parent)
 : QObject( parent )
{
}

Patient::~Patient()
{
    m_studiesSet.clear();
}

void Patient::setFullName( QString name )
{
    m_fullName = name;
}

void Patient::setID( QString id )
{
    m_patientID = id;
}

void Patient::setBirthDate( int day , int month , int year )
{
    m_birthDate.setYMD( year , month , day );
}

QString Patient::getBirthDate()
{
    return m_birthDate.toString( "dd/MM/yyyy" );
}

int Patient::getDayOfBirth()
{
    return m_birthDate.day();
}

int Patient::getMonthOfBirth()
{
    return m_birthDate.month();
}

int Patient::getYearOfBirth()
{
    return m_birthDate.year();
}

void Patient::setSex( QString sex )
{
    m_sex = sex;
}

bool Patient::addStudy( Study *study )
{
    bool ok = true;
    QString uid = study->getInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de l'estudi està buit! No el podem insertar per inconsistent");
    }
    else if( m_studiesSet.contains( uid ) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix un estudi amb aquest mateix UID:: " + uid );
    }
    else
    {
        m_studiesSet[ uid ] = study;
        study->setParentPatient( this );
    }

    return ok;
}

void Patient::removeStudy( QString uid )
{
    m_studiesSet.remove( uid );
}

Study *Patient::getStudy( QString uid )
{
    if( m_studiesSet.contains( uid ) )
        return m_studiesSet[ uid ];
    else
        return 0;
}

bool Patient::studyExists( QString uid )
{
    if( this->getStudy(uid) )
        return true;
    else
        return false;
}

int Patient::getNumberOfStudies()
{
    return m_studiesSet.size();
}

QList<Study*> Patient::getStudies()
{
    QList< Study* > studyList;

    QHashIterator<QString, Study *> iterator( m_studiesSet );
    while( iterator.hasNext() )
    {
        studyList << iterator.value();
        iterator.next();
    }

    return studyList;
}

Patient *Patient::operator +( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

Patient *Patient::operator +=( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

Patient *Patient::operator -( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

Patient *Patient::operator -=( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

}
