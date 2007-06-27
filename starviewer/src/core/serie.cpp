/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "serie.h"

namespace udg {

Serie::Serie(QObject *parent)
 : QObject(parent)
{
}

Serie::~Serie()
{
}

void Serie::setUID( QString uid )
{
    m_serieUID = uid;
}

void Serie::setID( QString id )
{
    m_serieID = id;
}

void Serie::setPatientOrientation( QString orientation )
{
    m_patientOrientation = orientation;
}

void Serie::setModality( QString modality )
{
    m_modality = modality;
}

void Serie::setDescription( QString description )
{
    m_description = description;
}

void Serie::setProtocolName( QString protocolName )
{
    m_protocolName = protocolName;
}

void Serie::setSeriesPath( QString seriesPath )
{
    m_seriesPath = seriesPath;
}

QString Serie::getKey()
{
    return m_protocolName + QString(" ") + m_description;
}

bool Serie::setDateTime( int day , int month , int year , int hour , int minute )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    m_dateTime.setTime( QTime( hour , minute ) );

    return m_dateTime.isValid();
}

bool Serie::setDateTime( QString date , QString time )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy" ) );
    m_dateTime.setTime( QTime::fromString( time , "hh:mm" ) );

    return m_dateTime.isValid();
}

bool Serie::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , hh:mm" );
    return m_dateTime.isValid();
}

QString Serie::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , hh:mm" );
}

bool Serie::setDate( int day , int month , int year )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    return m_dateTime.isValid();
}

bool Serie::setDate( QString date )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy") );
    return m_dateTime.isValid();
}

bool Serie::setTime( int hour , int minute )
{
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool Serie::setTime( QString time )
{
    m_dateTime.setTime( QTime::fromString( time , "hh:mm") );
    return m_dateTime.isValid();
}

QDate Serie::getDate()
{
    return m_dateTime.date();
}

QString Serie::getDateAsString()
{
    return m_dateTime.date().toString( "dd/MM/yyy" );
}

QTime Serie::getTime()
{
    return m_dateTime.time();
}

QString Serie::getTimeAsString()
{
    return m_dateTime.time().toString( "hh:mm" );
}

void Serie::setWindowLevel( double window , double level )
{
    m_window = window;
    m_level = level;
}

double *Serie::getWindowLevel()
{
    static double wl[2] = { m_window , m_level };
    return wl;
}

void Serie::setWindow( double window )
{
    m_window = window;
}

void Serie::setLevel( double level )
{
    m_level = level;
}

void Serie::setDimensions( double dimensions[3] )
{
    m_dimensions[0] = dimensions[0];
    m_dimensions[1] = dimensions[1];
    m_dimensions[2] = dimensions[2];
}

void Serie::setDimensions( double x , double y , double z )
{
    m_dimensions[0] = x;
    m_dimensions[1] = y;
    m_dimensions[2] = z;
}

double *Serie::getDimensions()
{
    return m_dimensions;
}

void Serie::getDimensions( double dimensions[3] )
{
    dimensions[0] = m_dimensions[0];
    dimensions[1] = m_dimensions[1];
    dimensions[2] = m_dimensions[2];
}

void Serie::setSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void Serie::setSpacing( double x , double y , double z )
{
    m_spacing[0] = x;
    m_spacing[1] = y;
    m_spacing[2] = z;
}

double *Serie::getSpacing()
{
    return m_spacing;
}

void Serie::getSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void Serie::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

}
