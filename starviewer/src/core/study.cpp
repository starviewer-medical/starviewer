/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "study.h"
#include "patient.h"
#include "logging.h"

#include <QStringList>

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

QString Study::getAccessionNumber() const
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

QString Study::getModalitiesAsSingleString() const
{
    return m_modalities.join("/");
}

QStringList Study::getModalities() const
{
    return m_modalities;
}

bool Study::setDateTime( int day , int month , int year , int hour , int minute, int second )
{
    return this->setDate( day, month, year ) && this->setTime( hour, minute, second );
}

bool Study::setDateTime( QString date , QString time )
{
    return this->setDate( date ) && this->setTime( time );
}

bool Study::setDate( int day , int month , int year )
{
    return this->setDate( QDate( year , month , day ) );
}

bool Study::setDate( QString date )
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format yyyy.MM.dd
    return this->setDate( QDate::fromString(date.remove("."), "yyyyMMdd") );
}

bool Study::setDate( QDate date )
{
    if( date.isValid() )
    {
        m_date = date;
        return true;
    }
    else if( !date.isNull() )
    {
        DEBUG_LOG("La data està en un mal format" );
        return false;
    }
}

bool Study::setTime( int hour , int minute, int second )
{
    return this->setTime( QTime(hour, minute, second) );
}

bool Study::setTime( QString time )
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    if (split.size() == 2) //té fracció al final
    {
        // Trunquem a milisegons i no a milionèssimes de segons
        convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
    }

    return this->setTime( convertedTime );
}

bool Study::setTime(QTime time)
{
    if (time.isValid())
    {
        m_time = time;
        return true;
    }
    else if( !time.isNull() )
    {
        DEBUG_LOG( "El time està en un mal format" );
        return false;
    }
}

QDate Study::getDate()
{
    return m_date;
}

QString Study::getDateAsString()
{
    return m_date.toString( Qt::LocaleDate );
}

QTime Study::getTime()
{
    return m_time;
}

QString Study::getTimeAsString()
{
    return m_time.toString( "HH:mm:ss" );
}

QDateTime Study::getDateTime()
{
    return QDateTime( m_date, m_time );
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
    else if( this->seriesExists(uid) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix una sèrie amb aquest mateix UID:: " + uid );
    }
    else
    {
        series->setParentStudy( this );
        this->insertSeries( series );
    }

    return ok;
}

void Study::removeSeries( QString uid )
{
    int index = this->findSeriesIndex(uid);
    if( index != -1 )
        m_seriesSet.removeAt( index );
}

Series *Study::getSeries( QString uid )
{
    int index = this->findSeriesIndex(uid);
    if( index != -1 )
        return m_seriesSet[index];
    else
        return NULL;
}

bool Study::seriesExists( QString uid )
{
    if( this->findSeriesIndex(uid) != -1 )
        return true;
    else
        return false;
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

int Study::getNumberOfSeries()
{
    return m_seriesSet.size();
}

QList< Series* > Study::getSeries()
{
    return m_seriesSet;
}

QString Study::toString()
{
    QString result;

    result += "    - Study\n";
    result += "        InstanceUID : " + getInstanceUID() + "\n";
    result += "        Description : " + getDescription() + "\n";

    foreach (Series *series, getSeries())
    {
        result += series->toString();
    }

    return result;
}

void Study::insertSeries( Series *series )
{
    int i = 0;
    while( i < m_seriesSet.size() && m_seriesSet.at(i)->getSeriesNumber().toInt() < series->getSeriesNumber().toInt() )
    {
        i++;
    }
    if( !m_modalities.contains( series->getModality() ) )
        m_modalities << series->getModality();

    m_seriesSet.insert( i, series );
}

int Study::findSeriesIndex( QString uid )
{
    int i = 0;
    bool found = false;
    while( i < m_seriesSet.size() && !found )
    {
        if( m_seriesSet.at(i)->getInstanceUID() == uid )
            found = true;
        else
            i++;
    }
    if( !found )
        i = -1;

    return i;
}

}
