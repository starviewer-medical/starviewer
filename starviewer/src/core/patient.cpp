/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patient.h"

namespace udg {

Patient::Patient(QObject *parent)
 : QObject( parent )
{
}

Patient::~Patient()
{
    m_studiesHash.clear();
}

void Patient::setName( QString name )
{
    m_name = name;
}

QString Patient::getName()
{
    return m_name;
}

void Patient::setSurname( QString surname )
{
    m_surname = surname;
}

QString Patient::getSurname()
{
    return m_surname;
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

void Patient::addStudy( Study *study )
{
    m_studiesHash[ study->getInstanceUID() ] = study;
}

void Patient::removeStudy( QString uid )
{
    m_studiesHash.remove( uid );
}

Study *Patient::getStudy( QString uid )
{
    return m_studiesHash[ uid ];
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
    return m_studiesHash.size();
}

QList<Study*> Patient::getStudies()
{
    QList< Study* > studyList;

    QHashIterator<QString, Study *> iterator( m_studiesHash );
    while( iterator.hasNext() )
    {
        studyList << iterator.value();
        iterator.next();
    }

    return studyList;
}
}
