/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "study.h"
#include "patient.h"
#include "logging.h"

namespace udg {

Study::Study( Patient *parentPatient, QObject *parent )
 : QObject(parent), m_parentPatient( parentPatient )
{
}

Study::~Study()
{
}

void Study::setParentPatient( Patient *patient )
{
    m_parentPatient = patient;
}

Patient* Study::getParentPatient() const
{
    return m_parentPatient;
}

void Study::setInstanceUID( QString uid )
{
    m_studyInstanceUID = uid;
}

QString Study::getInstanceUID() const
{
    return m_studyInstanceUID;
}

void Study::setID( QString id )
{
    m_studyID = id;
}

QString Study::getID() const
{
    return m_studyID;
}

void Study::setAccessionNumber( QString accessionNumber )
{
    m_accessionNumber = accessionNumber;
}

QString Study::getAccesssionNumber() const
{
    return m_accessionNumber;
}

void Study::setDescription( QString description )
{
    m_description = description;
}

QString Study::getDescription() const
{
    return m_description;
}

void Study::setPatientAge( short int age )
{
    m_age = age;
}

short int Study::getPatientAge() const
{
    return m_age;
}

void Study::setWeight( double weight )
{
    m_weight = weight;
}

double Study::getWeight() const
{
    return m_weight;
}

void Study::setHeight( double height )
{
    m_height = height;
}

double Study::getHeight() const
{
    return m_height;
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
    m_dateTime.setTime( QTime::fromString( time , "HH:mm" ) );

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
    m_dateTime.setTime( QTime::fromString( time , "HH:mm") );
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
    return m_dateTime.time().toString( "HH:mm" );
}

bool Study::addSeries( Series *series )
{
    bool ok = true;
    QString uid = series->getInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de la sèrie està buida! No la podem insertar per inconsistent");
    }
    else if( m_seriesSet.contains( uid ) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix una sèrie amb aquest mateix UID:: " + uid );
    }
    else
    {
        m_seriesSet[ uid ] = series;
        series->setParentStudy( this );
    }

    return ok;
}

void Study::removeSeries( QString uid )
{
    m_seriesSet.remove( uid );
}

Series *Study::getSeries( QString uid )
{
    if( m_seriesSet.contains( uid ) )
        return m_seriesSet[ uid ];
    else
        return 0;
}

QList<Series *> Study::getSelectedSeries()
{
    QList<Series *> seriesList;
    foreach( Series *series, m_seriesSet )
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

    QHashIterator<QString, Series *> iterator( m_seriesSet );
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
    return m_seriesSet.size();
}

QList< Series* > Study::getSeries()
{
    return m_seriesSet.values();
}

}
