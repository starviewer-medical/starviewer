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

QString Series::getModality() const
{
    return m_modality;
}

void Series::setSeriesNumber( QString number )
{
    m_seriesNumber = number;
}

QString Series::getSeriesNumber() const
{
    return m_seriesNumber;
}

void Series::setFrameOfReferenceUID( QString uid )
{
    m_frameOfReferenceUID = uid;
}

QString Series::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

void Series::setPositionReferenceIndicator( QString position )
{
    m_positionReferenceIndicator = position;
}

QString Series::getPositionReferenceIndicator() const
{
    return m_positionReferenceIndicator;
}

void Series::setDescription( QString description )
{
    m_description = description;
}

QString Series::getDescription() const
{
    return m_description;
}

void Series::setPatientPosition( QString position )
{
    m_patientPosition = position;
}

QString Series::getPatientPosition() const
{
    return m_patientPosition;
}

void Series::setProtocolName( QString protocolName )
{
    m_protocolName = protocolName;
}

QString Series::getProtocolName() const
{
    return m_protocolName;
}

void Series::setImagesPath( QString imagesPath )
{
    m_imagesPath = imagesPath;
}

QString Series::getImagesPath() const
{
    return m_imagesPath;
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

QDateTime Series::getDateTime() const
{
    return m_dateTime;
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

QString Series::getInstitutionName() const
{
    return m_institutionName;
}

void Series::setVolumeIdentifier( Identifier id )
{
    m_volumeID = id;
}

Identifier Series::getVolumeIdentifier() const
{
    return m_volumeID;
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

bool Series::isSelected() const
{
    return m_selected;
}

void Series::setVolume( Volume * volume )
{
    m_volumesList.push_back(volume);
}

QString Series::toString(bool verbose)
{
    QString result;

    result += "        - Series\n";
    result += "            SeriesNumber : " + getSeriesNumber() + "\n";
    result += "            Modality : " + getModality() + "\n";
    result += "            Description : " + getDescription() + "\n";
    result += "            ProtocolName : " + getProtocolName() + "\n";
    result += "            Num.Images : " + QString::number( getImages().size() ) + "\n";

    if (verbose)
    {
        foreach (Image *image, getImages())
        {
            result += "            - Image " + image->getPath() + "\n";
        }
    }
    // TODO Idem per PS, KIN....

    return result;
}

void Series::select()
{
    setSelectStatus( true );
}

void Series::unSelect()
{
    setSelectStatus( false );
}

void Series::setSelectStatus( bool select )
{
    m_selected = select;
}

}
