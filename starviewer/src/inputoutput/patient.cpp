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
}

void Patient::setName( const char *name )
{
    m_name = QString::fromLatin1( name );
}

void Patient::setName( QString name )
{
    m_name = name;
}

QString Patient::getName()
{
    return m_name;
}

void Patient::setSurname( const char *surname )
{
    m_surname = QString::fromLatin1( surname );
}

void Patient::setSurname( QString surname )
{
    m_surname = surname;
}

QString Patient::getSurname()
{
    return m_surname;
}

void Patient::setDateOfBirth( int day , int month , int year )
{
    m_dateOfBirth.setYMD( year , month , day );
}

QString Patient::getDateOfBirth()
{
    return m_dateOfBirth.toString( "dd/MM/yyyy" );
}
    
int Patient::getDayOfBirth()
{
    return m_dateOfBirth.day();
}

int Patient::getMonthOfBirth()
{
    return m_dateOfBirth.month();
}

int Patient::getYearOfBirth()
{
    return m_dateOfBirth.year();
}

int Patient::getAge()
{
    return QDate::currentDate().year() - m_dateOfBirth.year();
}
    
void Patient::setWeight( double weight )
{
    m_weight = weight;
}

void Patient::setHeight( double height )
{
    m_height = height;
}

}
