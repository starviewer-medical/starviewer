/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "series.h"

#include "image.h"
#include "logging.h"

#include <QStringList>

namespace udg {

Series::Series(QObject *parent)
 : QObject(parent)
{
}

Series::~Series()
{
}

void Series::setInstanceUID( QString uid )
{
    m_seriesInstanceUID = uid;
}

void Series::addImage( Image *image )
{
    m_imageList.append( image );
    image->setParentSeries( this );
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

void Series::setImagesPath( QString imagesPath )
{
    m_imagesPath = imagesPath;
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

void Series::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

QStringList Series::getImagesPathList()
{
    QStringList pathList;
    foreach( Image *image, m_imageList )
    {
        pathList << image->getPath();
    }

    return pathList;
}

}
