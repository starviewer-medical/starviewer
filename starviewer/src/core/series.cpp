/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "series.h"

namespace udg {

Series::Series(QObject *parent)
 : QObject(parent)
{
}

Series::~Series()
{
}

void Series::setUID( QString uid )
{
    m_seriesUID = uid;
}

void Series::setID( QString id )
{
    m_seriesID = id;
}

void Series::setPatientOrientation( QString orientation )
{
    m_patientOrientation = orientation;
}

void Series::setModality( QString modality )
{
    m_modality = modality;
}

void Series::setDescription( QString description )
{
    m_description = description;
}

void Series::setProtocolName( QString protocolName )
{
    m_protocolName = protocolName;
}

void Series::setSeriessPath( QString seriessPath )
{
    m_seriessPath = seriessPath;
}

QString Series::getKey()
{
    return m_protocolName + QString(" ") + m_description;
}

bool Series::setDateTime( int day , int month , int year , int hour , int minute )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    m_dateTime.setTime( QTime( hour , minute ) );

    return m_dateTime.isValid();
}

bool Series::setDateTime( QString date , QString time )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy" ) );
    m_dateTime.setTime( QTime::fromString( time , "hh:mm" ) );

    return m_dateTime.isValid();
}

bool Series::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , hh:mm" );
    return m_dateTime.isValid();
}

QString Series::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , hh:mm" );
}

bool Series::setDate( int day , int month , int year )
{
    m_dateTime.setDate( QDate( year , month , day ) );
    return m_dateTime.isValid();
}

bool Series::setDate( QString date )
{
    m_dateTime.setDate( QDate::fromString( date , "dd/MM/yyyy") );
    return m_dateTime.isValid();
}

bool Series::setTime( int hour , int minute )
{
    m_dateTime.setTime( QTime( hour , minute ) );
    return m_dateTime.isValid();
}

bool Series::setTime( QString time )
{
    m_dateTime.setTime( QTime::fromString( time , "hh:mm") );
    return m_dateTime.isValid();
}

QDate Series::getDate()
{
    return m_dateTime.date();
}

QString Series::getDateAsString()
{
    return m_dateTime.date().toString( "dd/MM/yyy" );
}

QTime Series::getTime()
{
    return m_dateTime.time();
}

QString Series::getTimeAsString()
{
    return m_dateTime.time().toString( "hh:mm" );
}

void Series::setWindowLevel( double window , double level )
{
    m_window = window;
    m_level = level;
}

double *Series::getWindowLevel()
{
    static double wl[2] = { m_window , m_level };
    return wl;
}

void Series::setWindow( double window )
{
    m_window = window;
}

void Series::setLevel( double level )
{
    m_level = level;
}

void Series::setDimensions( double dimensions[3] )
{
    m_dimensions[0] = dimensions[0];
    m_dimensions[1] = dimensions[1];
    m_dimensions[2] = dimensions[2];
}

void Series::setDimensions( double x , double y , double z )
{
    m_dimensions[0] = x;
    m_dimensions[1] = y;
    m_dimensions[2] = z;
}

double *Series::getDimensions()
{
    return m_dimensions;
}

void Series::getDimensions( double dimensions[3] )
{
    dimensions[0] = m_dimensions[0];
    dimensions[1] = m_dimensions[1];
    dimensions[2] = m_dimensions[2];
}

void Series::setSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void Series::setSpacing( double x , double y , double z )
{
    m_spacing[0] = x;
    m_spacing[1] = y;
    m_spacing[2] = z;
}

double *Series::getSpacing()
{
    return m_spacing;
}

void Series::getSpacing( double spacing[3] )
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

void Series::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

}
