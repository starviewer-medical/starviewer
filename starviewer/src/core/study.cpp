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

void Study::setInstanceUID( QString uid )
{
    m_studyInstanceUID = uid;
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

void Study::addSeries( Series *series )
{
    m_seriesHash[ series->getInstanceUID() ] = series;
}

void Study::removeSeries( QString uid )
{
    m_seriesHash.remove( uid );
}

Series *Study::getSeries( QString uid )
{
    return m_seriesHash[ uid ];
}

QList<Series *> Study::getSelectedSeries()
{
    QList<Series *> seriesList;
    foreach( Series *series, m_seriesHash )
    {
        if( series->isSelected() )
            seriesList.append( series );
    }
    return seriesList;
}

Series *Study::getSeries( int index )
{
    int i = 0;
    Series *series = 0;

    QHashIterator<QString, Series *> iterator( m_seriesHash );
    while( iterator.hasNext() && i < index )
    {
        if( i == index )
            series = iterator.value();
        iterator.next();
        i++;
    }
    return series;
}

int Study::getNumberOfSeries()
{
    return m_seriesHash.size();
}

QList< Series* > Study::getSeries()
{
    QList< Series* > seriesList;

    QHashIterator<QString, Series *> iterator( m_seriesHash );
    while( iterator.hasNext() )
    {
        seriesList << iterator.value();
        iterator.next();
    }

    return seriesList;
}

}
