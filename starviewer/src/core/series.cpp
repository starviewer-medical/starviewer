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

void Series::setParentStudy( Study *study )
{
    m_parentStudy = study;
}

bool Series::addImage( Image *image )
{
    bool ok = true;
    QString uid = image->getSOPInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de la imatge està buit! No la podem insertar per inconsistent");
    }
    else if( m_imageSet.contains( uid ) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix una imatge amb aquest mateix SOPInstanceUID:: " + uid );
    }
    else
    {
        m_imageSet[ uid ] = image;
        image->setParentSeries( this );
    }

    return ok;
}

Image *Series::getImage( QString SOPInstanceUID )
{
    if( m_imageSet.contains( SOPInstanceUID ) )
        return m_imageSet[ SOPInstanceUID ];
    else
        return 0;
}

QList<Image *> Series::getImages()
{
    return m_imageSet.values();
}

bool Series::hasImages() const
{
    return ! m_imageSet.isEmpty();
}

void Series::setModality( QString modality )
{
    m_modality = modality;
}

void Series::setSeriesNumber( QString number )
{
    m_seriesNumber = number;
}

void Series::setFrameOfReferenceUID( QString uid )
{
    m_frameOfReferenceUID = uid;
}

void Series::setPositionReferenceIndicator( QString position )
{
    m_positionReferenceIndicator = position;
}

void Series::setDescription( QString description )
{
    m_description = description;
}

void Series::setPatientPosition( QString position )
{
    m_patientPosition = position;
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
    m_dateTime.setTime( QTime::fromString( time , "HH:mm" ) );

    return m_dateTime.isValid();
}

bool Series::setDateTime( QString dateTime )
{
    m_dateTime.fromString( dateTime , "dd/MM/yyyy , HH:mm" );
    return m_dateTime.isValid();
}

QString Series::getDateTimeAsString()
{
    return m_dateTime.toString( "dd/MM/yyyy , HH:mm" );
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
    m_dateTime.setTime( QTime::fromString( time , "HH:mm") );
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
    return m_dateTime.time().toString( "HH:mm" );
}

void Series::setInstitutionName( QString institutionName )
{
    m_institutionName = institutionName;
}

void Series::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

void Series::addFilePath(QString filePath)
{
    if (!m_filesPathList.contains(filePath))
    {
        m_filesPathList.append(filePath);
    }
}

void Series::removeFilePath(QString filePath)
{
    int i = m_filesPathList.indexOf(filePath);
    if (i != -1)
    {
        m_filesPathList.removeAt(i);
    }
}

QStringList Series::getFilesPathList()
{
    return m_filesPathList;
}

QStringList Series::getImagesPathList()
{
    QStringList pathList;
    foreach( Image *image, m_imageSet )
    {
        pathList << image->getPath();
    }

    return pathList;
}

}
