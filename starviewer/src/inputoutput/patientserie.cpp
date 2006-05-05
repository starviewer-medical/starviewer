/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientserie.h"

namespace udg {

PatientSerie::PatientSerie(QObject *parent)
 : QObject(parent)
{
}

PatientSerie::~PatientSerie()
{
}

void PatientSerie::setUID( const char *uid )
{
    m_serieUID = QString::fromLatin1( uid );
}

void PatientSerie::setID( const char *id )
{
    m_serieID = QString::fromLatin1( id );
}

void PatientSerie::setPatientOrientation( const char *orientation )
{
    m_patientOrientation = orientation;
}

void PatientSerie::setModality( const char *modality )
{
    m_modality = QString::fromLatin1( modality );
}

void PatientSerie::setDescription( const char *description )
{
    m_description = QString::fromLatin1( description );
}

void PatientSerie::setProtocolName( const char *protocolName )
{
    m_protocolName = QString::fromLatin1( protocolName );
}

void PatientSerie::setSeriesPath( const char *seriesPath )
{
    m_seriesPath = QString::fromLatin1( seriesPath );
}

QString PatientSerie::getKey()
{
    return m_protocolName + QString(" ") + m_description;
}

bool PatientSerie::setDateTime( int day , int month , int year , int hour , int minute )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    m_dateTime.setTime( QTime( hour , minute ) );

    return m_dateTime.isValid();
}

bool PatientSerie::setDateTime( QString date , QString time )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy" ) );
    m_dateTime.setTime( QTime::fromString( time , "hh:mm" ) );

    return m_dateTime.isValid();
}

bool PatientSerie::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , hh:mm" );
    return m_dateTime.isValid();
}

QString PatientSerie::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , hh:mm" );
}

bool PatientSerie::setDate( int day , int month , int year )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    return m_dateTime.isValid();
}

bool PatientSerie::setDate( QString date )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy") );
    return m_dateTime.isValid();
}

bool PatientSerie::setTime( int hour , int minute )
{
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool PatientSerie::setTime( QString time )
{
    m_dateTime.setTime( QTime::fromString( time , "hh:mm") );
    return m_dateTime.isValid();
}

QDate PatientSerie::getDate()
{
    return m_dateTime.date();
}

QString PatientSerie::getDateAsString()
{
    return m_dateTime.date().toString( "dd/MM/yyy" );
}

QTime PatientSerie::getTime()
{
    return m_dateTime.time();
}

QString PatientSerie::getTimeAsString()
{
    return m_dateTime.time().toString( "hh:mm" );
}

void PatientSerie::setWindowLevel( double window , double level )
{
    m_window = window;
    m_level = level;
}

double *PatientSerie::getWindowLevel()
{
    double wl[2] = { m_window , m_level };
    return wl;
}

void PatientSerie::setWindow( double window )
{
    m_window = window;
}

void PatientSerie::setLevel( double level )
{
    m_level = level;
}

void PatientSerie::setDimensions( double dimensions[3] )
{
    m_dimensions[0] = dimensions[0];
    m_dimensions[1] = dimensions[1];
    m_dimensions[2] = dimensions[2];
}

void PatientSerie::setDimensions( double x , double y , double z )
{
    m_dimensions[0] = x;
    m_dimensions[1] = y;
    m_dimensions[2] = z;
}

double *PatientSerie::getDimensions()
{
    return m_dimensions;
}

void PatientSerie::getDimensions( double dimensions[3] )
{
    dimensions[0] = m_dimensions[0];
    dimensions[1] = m_dimensions[1];
    dimensions[2] = m_dimensions[2];
}

void PatientSerie::setSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void PatientSerie::setSpacing( double x , double y , double z )
{
    m_spacing[0] = x;
    m_spacing[1] = y;
    m_spacing[2] = z;
}

double *PatientSerie::getSpacing()
{
    return m_spacing;
}

void PatientSerie::getSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void PatientSerie::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

}
