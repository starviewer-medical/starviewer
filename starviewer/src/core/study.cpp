/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "study.h"
#include "patient.h"

namespace udg {

Study::Study( Patient *parentPatient, QObject *parent )
 : QObject(parent), m_parentPatient( parentPatient )
{
}

Study::~Study()
{
}

void Study::setUID( QString uid )
{
    m_studyUID = uid;
}

void Study::setID( QString id )
{
    m_studyID = id;
}

void Study::setAccessionNumber( QString accessionNumber )
{
    m_accessionNumber = accessionNumber;
}

void Study::setDescription( QString description )
{
    m_description = description;
}

void Study::setInstitutionName( QString institutionName )
{
    m_institutionName = institutionName;
}

QString Study::getKey()
{
    return this->getDateTimeAsString() + QString(" ") + m_description;
}

bool Study::setDateTime( int day , int month , int year , int hour , int minute )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool Study::setDateTime( QString date , QString time )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy" ) );
    m_dateTime.setTime( QTime::fromString( time , "hh:mm" ) );

    return m_dateTime.isValid();
}

bool Study::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , hh:mm" );
    return m_dateTime.isValid();
}

QString Study::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , hh:mm" );
}

bool Study::setDate( int day , int month , int year )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    return m_dateTime.isValid();
}

bool Study::setDate( QString date )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy") );
    return m_dateTime.isValid();
}

bool Study::setTime( int hour , int minute )
{
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool Study::setTime( QString time )
{
    m_dateTime.setTime( QTime::fromString( time , "hh:mm") );
    return m_dateTime.isValid();
}

QDate Study::getDate()
{
    return m_dateTime.date();
}

QString Study::getDateAsString()
{
    return m_dateTime.date().toString( "dd/MM/yyyy" );
}

QTime Study::getTime()
{
    return m_dateTime.time();
}

QString Study::getTimeAsString()
{
    return m_dateTime.time().toString( "hh:mm" );
}

void Study::addSerie( Serie *serie )
{
    m_seriesHash[ serie->getKey() ] = serie;
}

void Study::removeSerie( QString key )
{
    m_seriesHash.remove( key );
}

Serie *Study::getSerie( QString key )
{
    return m_seriesHash[ key ];
}

}
