/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientstudy.h"
#include "patient.h"

namespace udg {

PatientStudy::PatientStudy( Patient *parentPatient, QObject *parent )
 : QObject(parent), m_parentPatient( parentPatient )
{
}

PatientStudy::~PatientStudy()
{
}

void PatientStudy::setUID( QString uid )
{
    m_studyUID = uid;
}

void PatientStudy::setID( QString id )
{
    m_studyID = id;
}

void PatientStudy::setAccessionNumber( QString accessionNumber )
{
    m_accessionNumber = accessionNumber;
}

void PatientStudy::setDescription( QString description )
{
    m_description = description;
}

void PatientStudy::setInstitutionName( QString institutionName )
{
    m_institutionName = institutionName;
}

QString PatientStudy::getKey()
{
    return this->getDateTimeAsString() + QString(" ") + m_description;
}

bool PatientStudy::setDateTime( int day , int month , int year , int hour , int minute )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool PatientStudy::setDateTime( QString date , QString time )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy" ) );
    m_dateTime.setTime( QTime::fromString( time , "hh:mm" ) );

    return m_dateTime.isValid();
}

bool PatientStudy::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , hh:mm" );
    return m_dateTime.isValid();
}

QString PatientStudy::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , hh:mm" );
}

bool PatientStudy::setDate( int day , int month , int year )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    return m_dateTime.isValid();
}

bool PatientStudy::setDate( QString date )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy") );
    return m_dateTime.isValid();
}

bool PatientStudy::setTime( int hour , int minute )
{
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool PatientStudy::setTime( QString time )
{
    m_dateTime.setTime( QTime::fromString( time , "hh:mm") );
    return m_dateTime.isValid();
}

QDate PatientStudy::getDate()
{
    return m_dateTime.date();
}

QString PatientStudy::getDateAsString()
{
    return m_dateTime.date().toString( "dd/MM/yyyy" );
}

QTime PatientStudy::getTime()
{
    return m_dateTime.time();
}

QString PatientStudy::getTimeAsString()
{
    return m_dateTime.time().toString( "hh:mm" );
}

void PatientStudy::addSerie( PatientSerie *patientSerie )
{
    m_seriesHash[ patientSerie->getKey() ] = patientSerie;
}

void PatientStudy::removeSerie( QString key )
{
    m_seriesHash.remove( key );
}

PatientSerie *PatientStudy::getSerie( QString key )
{
    return m_seriesHash[ key ];
}

}
